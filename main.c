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
	if(!js_init_interface()) {
		printf("Failed to initialize interface\n");
		return -1;
	}

	js_draw_main_menu();

	js_free_interface();
	printf("Program will quit\n");
	
	return 0;
}
