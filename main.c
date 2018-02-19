 // 
// Filename: main.c
// Created: 2018-02-13 18:49:32 +0100
// Author: Felix Nared
//

#include <stdio.h>
#include <stdbool.h>

#include "build.h"
#include "interface.h"

#include "emacs_ac_break.h"

int main(int argc, char *argv[])
{
#ifdef JS_DEBUG
	js_debug_init_log(NULL);		
	
#endif /* JS_DEBUG */
	
	if(!js_init_interface()) {
		printf("Failed to initialize interface\n");
		return -1;
	}

	js_draw_main_menu();

	js_free_interface();
	printf("Program will quit\n");

#ifdef JS_DEBUG
	js_debug_close_log();		
	
#endif /* JS_DEBUG */
	
	return 0;
}
