// 
// Filename: preproc.c
// Created: 2018-02-10 11:23:39 +0100
// Author: Felix Nared
//
// BUILD:
//   gcc preproc.c -o preproc.o
//
// MISC:
//   NULL is part of stdio.h (or any of it's subs).
//

// #define JS_ENABLE

#include <stdio.h>

#ifdef JS_ENABLE

#define JS_PRINT_VALUE(var, type) \
	printf(#var ": " type "\n", var)

#define JS_DEBUG_PRINT(func, msg, file, line) \
	printf("DEBUG " #func ": %s - File: %s (%d)\n", msg, file, line);

#define JS_DEBUG_NULLPTR(ptr, func, file, line)	\
	if(ptr == NULL) \
		JS_DEBUG_PRINT("DEBUG " func, "null pointer (" #ptr ")", file, line)

#else

#define JS_PRINT_VALUE(var, type)
#define JS_DEBUG_PRINT(func, msg, file, line)
#define JS_DEBUG_NULLPTR(ptr, func, file, line)

#endif /* JS_ENABLE */

int main(int argc, char *argv[])
{
	int i = 78;
	float f = 3.14;
	char str[] = "some text\n";

	JS_PRINT_VALUE(i, "%d");
	JS_PRINT_VALUE(i, "%X");
	JS_PRINT_VALUE(f, "%f");
	JS_PRINT_VALUE(str, "%s");

	char *n = NULL;
	
	JS_DEBUG_NULLPTR(n, main, __FILE__, __LINE__);
		
	return 0;
}
