// 
// Filename: interface.c
// Created: 2018-02-13 22:16:58 +0100
// Author: Felix Nared
//

#include <stdbool.h>

#include <ncurses.h>

#include "interface.h"
#include "tetris.h"

#include <stdio.h>
#include <string.h>
#include "emacs_ac_break.h"


static char input = 0;
static bool quit = false;


bool js_init_interface()
{
	initscr();
	raw();
	noecho();

	start_color();
	init_pair(1, COLOR_RED, COLOR_BLUE);
	
	return true;
}

void js_free_interface()
{
	quit = true;
	endwin();
}

void js_wait_for_input()
{
	input = getch();

	if(input == 'q')
		js_set_program_quit(true);
}

bool js_program_will_quit()
{
	return quit;
}

void js_set_program_quit(bool value)
{
	quit = value;
}

void js_draw_title_screen()
{
	WINDOW *title_window, *console_window;
	int x, w;
	int row, col;
	char *title = "JUST TETRIS";
	
	clear();

	mvprintw(7, 10, "Character pressed: ");
	
	if(input)
		addch(input | A_BOLD);       
	
	refresh();

	x = getmaxx(stdscr) / 2 - 8;
	w = strlen(title) + 6;
	init_pair(1, COLOR_RED, COLOR_BLUE);
	
	title_window = newwin(3, w, 3, x);

	wbkgd(title_window, COLOR_PAIR(1));
	box(title_window, 0, 0);
	wattrset(title_window, A_BOLD);
	
	mvwaddstr(title_window, 1, 3, title);
	
	wrefresh(title_window);

	getmaxyx(stdscr, row, col);
	console_window = newwin(1, col, row - 2, 0);

	wprintw(console_window, "screen: [%d, %d], center: %d, title width: %d\n",
		col, row, x, w);

	wrefresh(console_window);
	
	delwin(title_window);
	delwin(console_window);
}

void js_draw_file()
{
	int rows;
	char name[64], c, prev = 0;
	FILE *fp;
	bool comment = false;
	
	clear();
	
	addstr("Enter file name: ");	

	attron(A_BOLD);
	echo();	
	getstr(name);
	attroff(A_BOLD);
	
	fp = fopen(name, "r");

	clear();
	move(0, 0);

	if(fp == NULL) {
		addstr("Error - failed to open file ");
		attron(A_BOLD);
		addstr(name);
		attroff(A_BOLD);
		
		getch();

		js_set_program_quit(true);
		return;
	}

	rows = getmaxy(stdscr);		
	
	while((c = getc(fp)) != EOF) {
		int y, x;
		
		getyx(stdscr, y, x);

		if(y == rows - 1) {
			attron(A_PROTECT);
			addstr("[ NEXT PAGE ]");
			attroff(A_PROTECT);

			refresh();

			getch();

			clear();
			move(0, 0);
		}
		
		if(c == '/' && prev == '/') {
			attron(A_ITALIC);
			comment = true;
		}
		
		if(c == '\n' && comment) {
			attroff(A_ITALIC);
			comment = false;
		}

		addch(c);
		prev = c;
	}		

	refresh();
	getch();	

	js_set_program_quit(true);
}
	
