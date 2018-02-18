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


static int fill_random_board(jsBoard *board)
{
	int i, count = 0;
	jsBlock *blocks = board->blocks;

	for(i = 0; i < JS_BOARD_BLOCK_AMOUNT; i++) {
		if(rand() % 4 == 0) {
			// JS_DEBUG_VALUE(fill_random_board, i, "%d");
			
			blocks[i].status = -1;
			count++;
		}
	}

	return count;
}

static void draw_board(const jsBoard *board, jsVec2i orig)
{
	WINDOW *border_window, *board_window;	
	int x, y;
	const jsBlock *blocks = board->blocks;	
	
	border_window = newwin(JS_BOARD_ROW_AMOUNT + 2, JS_BOARD_COLUMN_AMOUNT + 2,
			       orig.y, orig.x);
	box(border_window, 0, 0);
	
	wrefresh(border_window);
	delwin(border_window);

	board_window = newwin(JS_BOARD_ROW_AMOUNT, JS_BOARD_COLUMN_AMOUNT,
			      orig.y + 1, orig.x + 1);
	
	init_pair(1, COLOR_BLUE, COLOR_RED);
	wattrset(board_window, COLOR_PAIR(1));

	for(y = 0; y < JS_BOARD_ROW_AMOUNT; y++) {
		for(x = 0; x < JS_BOARD_COLUMN_AMOUNT; x++) {
			if(!js_block_is_empty(board->pos[y][x]))
				mvwaddch(board_window, y, x, '#');
		}
	}	

	wrefresh(board_window);
	delwin(board_window);
}
	


int main(int argc, char *argv[])
{
	jsBoard board;
	int count, c = 0;
	
#ifdef JS_DEBUG
	js_debug_init_log("drawboard_log");
#endif // JS_DEBUG

	srand(time(NULL));
	
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	start_color();

	addstr("JUST TETRIS");
	refresh();

	while(c < 3) {
		board = js_empty_board();
		
		// count = fill_random_board(&board);	
		// JS_DEBUG_VALUE(main, count, "%d");

		board.pos[1 + c][2].status = -1;
		board.pos[2 + c][3].status = -1;
		board.pos[3 + c][4].status = -1;
		board.pos[4 + c][5].status = -1;
	
		draw_board(&board, (jsVec2i) {2, 2});

		getch();

		JS_DEBUG_VALUE(main, c, "%d");
		c++;
	}

	endwin();

#ifdef JS_DEBUG
	js_debug_close_log();
#endif // JS_DEBUG
	
	return 0;
}
