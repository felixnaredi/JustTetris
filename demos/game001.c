//
// Filename: game001.c
// Created: 2018-03-16 13:13:10 +0100
// Author: Felix Nared
//
// BUILD:
//	gcc game001.c -o game001.o ../source/vector.c ../source/tetris.c ../source/ruleset.c ../source/debug.c -lncurses -DJS_DEBUG

#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../source/tetris.h"
#include "../source/debug.h"
#include "../source/ruleset.h"


static WINDOW *window_board = NULL;
static WINDOW *window_active_board = NULL;
static WINDOW *window_score = NULL;
static WINDOW *window_level = NULL;


static void __js_init_ncurses()
{
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();

	init_pair(1, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);

	addstr("Just Tetris - " __FILE__);
	refresh();
}

static void __js_end_ncurses()
{
	if(window_active_board != NULL)
		delwin(window_active_board);

	if(window_board != NULL)
		delwin(window_board);

	endwin();
}

static int __js_make_board_window(jsVec2i position)
{
	window_board = newwin(JS_BOARD_ROW_AMOUNT + 2, JS_BOARD_COLUMN_AMOUNT + 2,
		              position.y, position.x);

	JS_DEBUG_NULLPTR(__js_make_board_window, window_board, error);

	window_active_board = subwin(window_board, JS_BOARD_ROW_AMOUNT,
	                             JS_BOARD_COLUMN_AMOUNT, position.y + 1,
			             position.x + 1);

	JS_DEBUG_NULLPTR(__js_make_board_window, window_active_board, error);

	return 1;
error:
	return 0;
}

static void __js_draw_block(WINDOW *window, jsBlock block, jsVec2i offset)
{
	int width, height, row, column;

	if(js_block_is_empty(block))
		return;

	getmaxyx(window, height, width);

	row = height - offset.y - 1 - block.position.y;
	column = offset.x + block.position.x;

	mvwaddch(window, row, column, '+');
}

static void __js_draw_shape(WINDOW *window, const jsShape *shape)
{
	int i;

	wattrset(window, COLOR_PAIR(2));

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++)
		__js_draw_block(window, shape->blocks[i], shape->offset);
}

static void __js_draw_board(WINDOW *window, const jsBoard *board)
{
	int i;
	const jsBlock *blocks = board->blocks;

	wattrset(window, COLOR_PAIR(1));

	for(i = 0; i < JS_BOARD_BLOCK_AMOUNT; i++)
		__js_draw_block(window, blocks[i], (jsVec2i){0, 0});
}

static void __js_draw_board_window(const jsBoard *board, const jsShape *shape)
{
	wclear(window_board);

	box(window_board, 0, 0);
	__js_draw_board(window_active_board, board);
	__js_draw_shape(window_active_board, shape);

	wrefresh(window_board);
}

static void __js_init_score_level_windows()
{
	window_score = newwin(2, 32, 4, 16);
}

static void __js_update_score_level(float score, float level)
{
	wclear(window_score);
	wprintw(window_score, "Score: %f\nLevel: %f", score, level);
	wrefresh(window_score);
}


typedef struct
{
	jsBoard board;
	jsShape shapes[2];
	int current;
} __jsBlockState;

static void __js_init_block_state(__jsBlockState *state)
{
	state->board = js_empty_board();
	state->shapes[0] = js_rand_shape();
	state->shapes[1] = js_rand_shape();
	state->current = 0;
}

static void __js_swap_shapes(__jsBlockState *state)
{
	int current = state->current;

	state->shapes[current] = js_rand_shape();
	state->current = (current + 1) % 2;
}

int main(int argc, char const *argv[]) {	
	__jsBlockState *state;
	jsBoard *board;
	int input;
	float score = 0, level = 0;

	jsRuleset rule = js_standard_ruleset();

#ifdef JS_DEBUG

	js_debug_init_log("log_demo_game");
#endif /* JS_DEBUG */

	state = malloc(sizeof(__jsBlockState));
	JS_DEBUG_NULLPTR(main, state, error);

	__js_init_ncurses();
	srand(time(NULL));

	__js_init_block_state(state);
	__js_init_score_level_windows();

	board = &state->board;

	if(!__js_make_board_window((jsVec2i){3, 3}))
		goto error;

	do {
		jsShape *shape = &state->shapes[state->current];
		jsVec2i vector = {0, 0};
		jsRotation direction = jsRotationNone;
		jsTranslationResult translation_result;
		jsRotationResult rotation_result;
		jsClearRowsResult clear_result;

		__js_draw_board_window(board, shape);

		input = getch();

		switch (input) {
		case KEY_UP:
			direction = jsRotationClockwise;
			break;
		case KEY_DOWN:
			vector = (jsVec2i){0, -1};
			break;
		case KEY_LEFT:
			vector = (jsVec2i){-1, 0};
			break;
		case KEY_RIGHT:
			vector = (jsVec2i){1, 0};
			break;
		default:
			break;
		}

		translation_result = js_translate_result(shape, board, vector);
		rotation_result = js_rotate_result(shape, board, direction);

		switch(translation_result.status | rotation_result.status) {
		case jsMoveStatusSuccess:
			*shape = js_translate_shape(shape, &translation_result);
			*shape = js_rotate_shape(shape, &rotation_result);

			score += rule.score_for_translation(translation_result) * rule.level_score_multiplier(level);
			break;
		case jsMoveStatusMerge:
			js_merge(board, shape);
			clear_result = js_clear_rows_result(board);
			js_clear_board_rows(board, &clear_result);

			__js_swap_shapes(state);

			level += rule.level_increment_for_clear(level, clear_result);
			score += rule.score_for_clear(clear_result) * rule.level_score_multiplier(level);
			break;
		default:
			break;
		}

		__js_update_score_level(score, level);

	} while(input != 'q');

	free(state);
	__js_end_ncurses();

#ifdef JS_DEBUG

	js_debug_close_log();
#endif /* JS_DEBUG */

	return 0;

error:
	return -1;
}
