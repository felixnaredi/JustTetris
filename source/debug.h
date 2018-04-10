//
// Filename: debug.h
// Created: 2018-04-10 11:37:15 +0200
// Author: Felix Nared
//

#ifdef JS_DEBUG

#include <stddef.h>

int js_debug_init_log(const char *path);
int js_debug_close_log();

int js_debug_print(const char *func, const char *file, int line,
                   const char *format, ...);

void
js_debug_dump(const char *func, const char *file, int line, const char *var_name,
              const void *pointer, size_t size);

#define JS_DEBUG_PUTS(func, msg) \
	js_debug_print(#func, __FILE__, __LINE__, msg)

#define JS_DEBUG_NULLPTR(func, ptr, label)		\
	if(ptr == NULL) { \
		js_debug_print(#func, __FILE__, __LINE__, "null pointer exception: " #ptr); \
		goto label; \
	}

#define JS_DEBUG_VALUE(func, var, type)		\
	js_debug_print(#func, __FILE__, __LINE__, #var ": " type, var)

#define JS_DEBUG_DUMP(func, var, size) \
	js_debug_dump(#func, __FILE__, __LINE__, #var, var, size)

#else

#define JS_DEBUG_PUTS(func, msg)
#define JS_DEBUG_NULLPTR(func, ptr, label)
#define JS_DEBUG_VALUE(func, var, type)
#define JS_DEBUG_DUMP(func, var, size)

#endif /* JS_DEBUG */
