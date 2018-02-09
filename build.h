// 
// Filename: build.h
// Created: 2018-02-10 00:35:36 +0100
// Author: Felix Nared
//

#ifndef BUILD_H
#define BUILD_H

// #define JS_BUILD_SAFE_GLOBAL

#ifdef JS_DEBUG

#define JS_DEBUG_NULLPTR(ptr, func, file, line)				\
	if(ptr == NULL) printf("DEBUG %s: null pointer at %s %d", func, file, line)

#endif /* JS_DEBUG */

#endif /* BUILD_H */


