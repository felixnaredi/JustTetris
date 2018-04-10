//
// Filename: debug.c
// Created: 2018-02-17 06:32:57 +0100
// Author: Felix Nared
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "debug.h"

#ifdef JS_USING_EMACS

#include "emacs_ac_break.h"
#endif /* JS_USING_EMACS */

static FILE *__js_debug_file = NULL;

static size_t __js_now(char *des, size_t len)
{
 	time_t now = time(NULL);
 	return strftime(des, len, "[%F %X]", localtime(&now));
}

/// Opens the file at path with 'append' option and sets it to the
/// current debug file.
///
/// Returns 0 value on failure.
int js_debug_init_log(const char *path)
{
	char now[32];

	if(__js_debug_file != NULL)
		return 0;

	if(path == NULL) {
		__js_debug_file = stdout;
		return 0;
	}

	__js_debug_file = fopen(path, "a");

	if(__js_debug_file == NULL) {
		printf("Could not open debug file (file set to stdout)\n");
		__js_debug_file = stdout;
		return 0;
	}

	__js_now(now, sizeof(now));

	fprintf(__js_debug_file, "-------- LOG INITIATED %s --------\n", now);

	return 1;
}

/// Closes the current debug file.
///
/// Returns 0 if no file were closed.
int js_debug_close_log()
{
	char now[32];

	if(__js_debug_file == NULL || __js_debug_file == stdout)
		return 0;

	__js_now(now, sizeof(now));

	fprintf(__js_debug_file, "-------- LOG CLOSED %s --------\n\n", now);

	fclose(__js_debug_file);
	__js_debug_file = NULL;

	return 1;
}

int js_debug_print(const char *func, const char *file, int line, const char *format, ...)
{
	va_list args;
	char msg[1028], now[32];

	va_start(args, format);
	vsprintf(msg, format, args);

	__js_now(now, sizeof(now));

	va_end(args);

	return fprintf(__js_debug_file, "%s DEBUG %s (%s, %d) - %s\n",
		       now, func, file, line, msg);
}

static size_t
__js_print_hex_dump(const void *pointer, size_t size, char *des, size_t len)
{
	const int8_t *p;
	size_t count = 0;

	for(p = (int8_t *)pointer; p < (int8_t *)(pointer + size); p++)
	{
		count += sprintf(&des[count], "%02hhX ", *p);
		if(count > len) {
			des[len] = '\0';
			return len;
		}
	}

	return count;
}

void
js_debug_dump(const char *func, const char *file, int line, const char *var_name,
              const void *pointer, size_t size)
{
	char buf[1028], *mbuf = NULL;
	size_t len = size * 3 + 1;

	if(len > sizeof(buf)) {
		mbuf = malloc(len);

		__js_print_hex_dump(pointer, size, mbuf, len);
		js_debug_print(func, file, line, "Dump %s: %s", var_name, mbuf);

		free(mbuf);
	} else {
		__js_print_hex_dump(pointer, size, buf, sizeof(buf));
		js_debug_print(func, file, line, "Dump %s: %s", var_name, buf);
	}
}
