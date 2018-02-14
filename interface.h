// 
// Filename: interface.h
// Created: 2018-02-13 22:15:23 +0100
// Author: Felix Nared
//


#include <stdbool.h>

#include "emacs_ac_break.h"

bool js_init_interface();
void js_free_interface();
void js_wait_for_input();
bool js_program_will_quit();
void js_set_program_quit(bool value);

void js_draw_title_screen();
void js_draw_file();


