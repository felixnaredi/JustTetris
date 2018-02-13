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

#define JS_DEBUG_PRINT(func, msg, file, line) \
	printf("DEBUG " #func ": %s - File: %s (%d)\n", msg, file, line);

#define JS_DEBUG_NULLPTR(ptr, func, file, line)	\
	if(ptr == NULL) \
		JS_DEBUG_PRINT("DEBUG " func, "null pointer (" #ptr ")", file, line)

#define JS_DEBUG_VALUE(var, type) \
	printf("DEBUG " #var ": " type "\n", var)

#else

#define JS_DEBUG_PRINT(func, msg, file, line)
#define JS_DEBUG_NULLPTR(ptr, func, file, line)
#define JS_DEBUG_VALUE(var, type)

#endif /* JS_DEBUG */

#ifdef JS_BUILD_SAFE_GLOBAL





#endif // JS_BUILD_SAFE_GLOBAL


#endif /* BUILD_H */


