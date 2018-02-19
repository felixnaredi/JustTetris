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

static int input = 0;
static bool quit = false;

bool js_init_interface()
{	
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

	start_color();
	
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

static void __js_draw_title_window(jsVec2i pos)
{
	WINDOW *window;
	int width;
	char *title = "JUST TETRIS";
	jsVec2i margin = {3, 1};

	width = strlen(title) + margin.x * 2;
	window = newwin(3, width, pos.y, pos.x);

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
	void (*select)(void *sender);
} __js_jsMenuItem;

static void __js_draw_menu_item(const __js_jsMenuItem *item, jsVec2i pos, bool selected)
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

static void __js_draw_menu_items(const __js_jsMenuItem *items, int count, int index, jsVec2i pos)
{
	int i;

	for(i = 0; i < count; i++) {
		__js_draw_menu_item(&items[i], pos, i == index);
		pos.y += 2;
	}
}

static void __js_item_name(void *sender)
{
	__js_jsMenuItem *item = (__js_jsMenuItem *) sender;
	JS_DEBUG_VALUE(__js_item_name, item->name, "%s");
}

static void __js_quit_program(void *sender)
{	
	js_set_program_quit(true);
}

void js_draw_main_menu()
{
	int cx;
	jsVec2i title_pos = {0, 2}, menu_pos = {0, 6};

	__js_jsMenuItem items[] = {
		{"New Game", __js_item_name},
		{"Quit", __js_quit_program},
	};
	int index = 0, count = 2;
	
	cx = getmaxx(stdscr) / 2;
	cx = cx > 11 ? 11 : cx;
	
	title_pos.x = cx;
	menu_pos.x = js_max(cx - 5, 3);

	while(!js_program_will_quit()) {
		__js_jsMenuItem item = items[index];
		
		clear();
		refresh();

		__js_draw_title_window(title_pos);
		__js_draw_menu_items(items, count, index, menu_pos);
		
		js_wait_for_input();

		switch(input) {
		case KEY_UP:
			index--;
			index = index < 0 ? count - 1 : index;
			break;
		case KEY_DOWN:
			index = (index + 1) % count;
			break;
		case KEY_ENTER:
		case '\n':			
			if(item.select == NULL)
				break;

			item.select(&item);
			
			break;
		default:
			break;
		}
	}

}
