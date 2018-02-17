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

static size_t __js_now(char *des, size_t len)
{
	time_t now = time(NULL);

	return strftime(des, len, "[%F %X]", localtime(&now));
}	

int js_debug_init_log(char *path) {
	char now[32];
	
	if(path == NULL)
		path = "log";

	__js_debug_file = fopen(path, "a");

	if(__js_debug_file == NULL) {
		__js_debug_file = stdout;
		return 0;
	}

	__js_now(now, sizeof(now));
	fprintf(__js_debug_file, "-------- NEW RUN %s --------\n", now);

	return 1;
}

int __js_debug_print(const char *func, const char *file, int line, const char *format, ...)
{	
	va_list args;
	char msg[128], now[32];

	va_start(args, format);
	vsprintf(msg, format, args);

	__js_now(now, sizeof(now));

	return fprintf(__js_debug_file, "%s DEBUG %s (%s, %d) - %s\n",
		       now, func, file, line, msg);
}
