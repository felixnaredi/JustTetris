//
// Filename: drawboard.c
// Created: 2018-02-18 05:17:05 +0100
// Author: Felix Nared
//
// BUILD:
//   gcc drawboard.c ../tetris.c ../math.c ../debug.c -o drawboard.o -lncurses -DJS_DEBUG

#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "../tetris.h"
#include "../build.h"
#include "../emacs_ac_break.h"


static void __draw_board(const jsBoard *board, WINDOW *window)
{
	int y, x;
	const jsBlock *blocks = board->blocks;

	init_pair(1, COLOR_BLUE, COLOR_RED);
	wattrset(window, COLOR_PAIR(1));

	for(y = 0; y < JS_BOARD_ROW_AMOUNT; y++) {
		for(x = 0; x < JS_BOARD_COLUMN_AMOUNT; x++) {
			int yp, xp;

			yp = JS_BOARD_ROW_AMOUNT - 1 - y;
			xp = x;

			if(!js_block_is_empty(board->pos[y][x]))
				mvwaddch(window, yp, xp, '#');
		}
	}
}

static void __draw_shape(const jsShape *shape, WINDOW *window)
{
	int i;
	jsVec2i offset = shape->offset;
	const jsBlock *blocks = shape->blocks;

	init_pair(2, COLOR_RED, COLOR_YELLOW);
	wattrset(window, COLOR_PAIR(2));

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		int x, y;
		jsBlock block = blocks[i];

		y = JS_BOARD_ROW_AMOUNT - offset.y - 1 - block.position.y;
		x = offset.x + block.position.x;

		mvwaddch(window, y, x, '#');
	}
}

static void __draw_state(const jsTetrisState *state, jsVec2i orig)
{
	WINDOW *border, *board;

	border = newwin(JS_BOARD_ROW_AMOUNT + 2, JS_BOARD_COLUMN_AMOUNT + 2, orig.y, orig.x);
	box(border, 0, 0);

	wrefresh(border);
	delwin(border);

	board = newwin(JS_BOARD_ROW_AMOUNT, JS_BOARD_COLUMN_AMOUNT, orig.y + 1, orig.x + 1);

	__draw_board(state->board, board);
	__draw_shape(state->shape, board);

	wrefresh(board);
	delwin(board);
}

static void __fall(jsTetrisState *state)
{
	do {
		js_move_shape(state, (jsVec2i) {0, -1});
		
	} while(!(state->status & JS_STATE_BOARD_CHANGE));
}

int main(int argc, char *argv[])
{
	jsTetrisState *state;
	int input;

#ifdef JS_DEBUG

	js_debug_init_log("log_drawboard");
#endif // JS_DEBUG

	srand(time(NULL));

	state = js_alloc_tetris_state();

	if(state == NULL) {
		fprintf(stderr, "failed to init state");
		return -1;
	}

	js_init_tetris_state(state);

	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();

	addstr("JUST TETRIS");
	refresh();

	do {
		__draw_state(state, (jsVec2i) {2, 2});

		input = getch();

		switch(input) {
		case KEY_UP:
			js_move_shape(state, (jsVec2i) {0, 1});
			break;
		case KEY_DOWN:
			js_move_shape(state, (jsVec2i) {0, -1});
			break;
		case KEY_RIGHT:
			js_move_shape(state, (jsVec2i) {1, 0});
			break;
		case KEY_LEFT:
			js_move_shape(state, (jsVec2i) {-1, 0});
			break;
		case 'r':
			js_rotate_shape(state, jsRotationClockwise);
			break;
		case 'e':
			js_rotate_shape(state, jsRotationCounterClockwise);
			break;
		case ' ':
			__fall(state);
			break;
		default:
			break;
		}

		if(state->status & JS_STATE_SCORE_CHANGE)
			JS_DEBUG_VALUE(main, state->score, "%f");

		if(state->status & JS_STATE_GAME_OVER) {
			JS_DEBUG_PUTS(main, "game over!");
			js_init_tetris_state(state);
		}

		js_clear_state_status(state);

	} while(input != 'q');

	endwin();
	js_dealloc_tetris_state(state);

#ifdef JS_DEBUG

	js_debug_close_log();
#endif // JS_DEBUG

	return 0;
}
