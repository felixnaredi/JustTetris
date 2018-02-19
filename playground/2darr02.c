// 
// Filename: 2darr02.c
// Created: 2018-02-17 21:29:58 +0100
// Author: Felix Nared
//
// BUILD:
//   gcc 2darr02.c -o 2darr02.o

#include <stdio.h>
#include "../emacs_ac_break.h"

typedef struct
{
	int a;
	int b;
} Item;

#define ROWS 20
#define COLS 10

typedef struct
{
	Item items[COLS];
} Row;

typedef union
{
	Item items[ROWS * COLS];
	Row rows[ROWS];
	
} TwoDim;
	
int main(int argc, char *argv[])
{
	TwoDim td;
	int i, y, x, s = 1;
	Item *items = td.items;

	for(i = 0; i < ROWS * COLS; i++) {
		Item *item = &items[i];
		
		item->a = i;
		item->b = i * 2;
	}

	i = 0;
	
	for(y = 0; y < ROWS; y++) {
		Row row = td.rows[y];
		
		for(x = 0; x < COLS; x++) {
			Item item = row.items[x];
			int b;
			
			printf("[%d, %d]{a: %d, b: %d} == ", x, y, item.a, item.b);
			printf("[%d]{a: %d, b: %d} ", i, items[i].a, items[i].b);

			b = item.a == items[i].a && item.b == items[i].b;
			printf("==> %s\n", b ? "true" : "false");

			s &= b;

			i++;
		}
	}

	if(s)
		printf("-- All results true\n");
	else
		printf("-- At least one false\n");
		
	return 0;
}
