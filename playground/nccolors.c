// 
// Filename: nccolors.c
// Created: 2018-02-17 22:35:00 +0100
// Author: Felix Nared
//
// BUILD:
//   gcc nccolors.c -o nccolors.o -lncurses
//

#include <stdio.h>
#include <ncurses.h>

#include "../emacs_ac_break.h"

int main(int argc, char *argv[])
{
	char buf[1024];
	int x, y;

	setbuf(stdout, buf);	

	initscr();
	start_color();

	printf("COLORS: %d\n", COLORS);
	printf("can_change_colors: %d\n", can_change_color());
	printf("COLOR_BLACK: %X\n", COLOR_BLACK);
	printf("COLOR_RED: %X\n", COLOR_RED);
	printf("COLOR_YELLOW: %X\n", COLOR_YELLOW);
	printf("COLOR_GREEN: %X\n", COLOR_GREEN);
	printf("COLOR_CYAN: %X\n", COLOR_CYAN);
	printf("COLOR_BLUE: %X\n", COLOR_BLUE);
	printf("COLOR_MAGENTA: %X\n", COLOR_MAGENTA);
	printf("COLOR_WHITE: %X\n", COLOR_WHITE);

	endwin();
	
	return 0;
}
