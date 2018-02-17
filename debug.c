// 
// Filename: debug.c
// Created: 2018-02-17 06:32:57 +0100
// Author: Felix Nared
//

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "build.h"

#include "emacs_ac_break.h"

static FILE *__js_debug_file;

static const char *__js_now()
{
	return "[2000-01-01 00:00:00]";
}	

int js_debug_init_log(char *path) {
	if(path == NULL)
		path = "log";

	__js_debug_file = fopen(path, "a");

	if(__js_debug_file == NULL) {
		__js_debug_file = stdout;
		return 0;
	}

	fprintf(__js_debug_file, "-------- NEW RUN %s --------\n", __js_now());

	return 1;
}

int __js_debug_print(const char *func, const char *file, int line, const char *format, ...)
{
	return fprintf(__js_debug_file, "%s DEBUG %s (%s, %d) - Some message\n",
		       __js_now(), func, file, line);
}
