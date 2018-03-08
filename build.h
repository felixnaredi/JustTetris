//
// Filename: build.h
// Created: 2018-02-10 00:35:36 +0100
// Author: Felix Nared
//

#ifndef BUILD_H
#define BUILD_H

#include <stdio.h>

// #define JS_DEBUG
// #define JS_BUILD_SAFE_GLOBAL
// #define JS_BUILD_RENDER_NCURSES
// #define JS_BUILD_RENDER_OPENGL

#ifdef JS_DEBUG

int js_debug_init_log(char *path);
int js_debug_close_log();

int __js_debug_print(const char *func, const char *file, int line, const char *format, ...);

#define JS_DEBUG_PUTS(func, msg) \
	__js_debug_print(#func, __FILE__, __LINE__, msg)

#define JS_DEBUG_NULLPTR(func, ptr, label)		\
	if(ptr == NULL) { \
		__js_debug_print(#func, __FILE__, __LINE__, "null pointer exception: " #ptr) \
		goto label; \
	}

#define JS_DEBUG_VALUE(func, var, type)		\
	__js_debug_print(#func, __FILE__, __LINE__, #var ": " type, var)

#else

#define JS_DEBUG_PUTS(func, msg)
#define JS_DEBUG_NULLPTR(func, ptr)
#define JS_DEBUG_VALUE(func, var, type)

#endif /* JS_DEBUG */

#ifdef JS_BUILD_SAFE_GLOBAL





#endif // JS_BUILD_SAFE_GLOBAL


#endif /* BUILD_H */
