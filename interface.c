// 
// Filename: interface.c
// Created: 2018-02-13 22:16:58 +0100
// Author: Felix Nared
//

#include <string.h>
#include <stdbool.h>

#include <ncurses.h>

#include "interface.h"
#include "tetris.h"
#include "build.h"

#include "emacs_ac_break.h"

static int __js_input = 0;
static bool __js_quit = false;

bool js_init_interface()
{	
	initscr();
	raw();
	noecho();

	start_color();
	
	return true;
}

void js_free_interface()
{
	__js_quit = true;
	endwin();
}

void js_wait_for_input()
{
	__js_input = getch();

	if(__js_input == 'q')
		js_set_program_quit(true);
}

bool js_program_will_quit()
{
	return __js_quit;
}

void js_set_program_quit(bool value)
{
	__js_quit = value;
}

static void __draw_title_window(jsVec2i pos)
{
	WINDOW *window;
	int width;
	char *title = "JUST TETRIS";
	jsVec2i margin = {3, 1};

	width = strlen(title) + margin.x * 2;
	window = newwin(3, width, pos.y, pos.x - width / 2);

	init_pair(1, COLOR_RED, COLOR_BLUE);
	wattrset(window, A_BOLD);

	wbkgd(window, COLOR_PAIR(1));
	box(window, 0, 0);
	mvwaddstr(window, margin.y, margin.x, title);

	wrefresh(window);
	delwin(window);
}

typedef struct
{
	char *name;
	void (*on_enter)(int);
} jsMenuItem;

static void __draw_menu_item(const jsMenuItem *item, jsVec2i pos, bool selected)
{
	const char *name = item->name;
	WINDOW *window = newwin(1, strlen(name), pos.y, pos.x);
	void *widget = NULL;

	if(selected)
		wattrset(window, A_REVERSE);

	waddstr(window, name);

	wrefresh(window);
	delwin(window);
}

static void __draw_menu_items(const jsMenuItem *items, int count, int index, jsVec2i pos)
{
	int i;

	for(i = 0; i < count; i++) {
		__draw_menu_item(&items[i], pos, i == index);
		pos.y += 2;
	}
}

void js_draw_main_menu()
{
	int cx;
	jsVec2i title_pos = {0, 2}, menu_pos = {0, 6};

	jsMenuItem items[] = {
		{"First Item", NULL},
		{"Second Item", NULL},
	};
	int index = 0, count = 2;
	
	cx = getmaxx(stdscr) / 2;
	
	title_pos.x = cx;
	menu_pos.x = cx - 10;

	JS_DEBUG_VALUE(js_draw_main_menu, KEY_UP, "%d");
	JS_DEBUG_VALUE(js_draw_main_menu, KEY_DOWN, "%d");

	while(!js_program_will_quit()) {
		clear();
		refresh();

		__draw_title_window(title_pos);
		__draw_menu_items(items, count, index, menu_pos);		
	
		js_wait_for_input();
		JS_DEBUG_VALUE(js_draw_main_menu, __js_input, "%d");

		switch(__js_input) {
		case KEY_UP:
			index--;
			index = index < 0 ? count : index;
			break;
		case KEY_DOWN:
			index = (index + 1) % count;
			break;
		default:
			break;
		}
	}

}
