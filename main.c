// 
// Filename: main.c
// Created: 2018-02-13 18:49:32 +0100
// Author: Felix Nared
//

#include <stdio.h>
#include <stdbool.h>

#include "interface.h"

#include "emacs_ac_break.h"

int main(int argc, char *argv[])
{
	int count = 0;	
	
	if(!js_init_interface()) {
		printf("Failed to initialize interface\n");
		return -1;
	}

	while(!js_program_will_quit()) {
		js_draw_title_screen();
		js_wait_for_input();

		// js_draw_file();

		if(count > 5)
			js_set_program_quit(true);
		
		count++;
	}

	js_free_interface();
	printf("Program will quit\n");
	
	return 0;
}
