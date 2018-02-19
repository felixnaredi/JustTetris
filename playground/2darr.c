// 
// Filename: 2darr.c
// Created: 2018-02-11 21:46:46 +0100
// Author: Felix Nared
//
// BUILD:
//   gcc 2darr.c -o 2darr.o
//   gcc -O3 2darr.c -o 2darr.o
//   clang 2darr.c -o 2darr.o
//   clang -O3  2darr.c -o 2darr.o
//
// RESULT:
//   On this system (Arch Linux GNU), compiled with anything above,
//   the memory will be packed. Or better but, there is no implicit
//   alignment. This makes the second 'for loop' in 'main' possible.
//

#include <stdio.h>
#include <string.h>

#define COLUMNS 10
#define ROWS 20
#define LENGTH ROWS * COLUMNS

typedef struct
{
	int value;
	int space;
	int pad;
} Value;

typedef struct
{
	Value values[COLUMNS];
} Row;

typedef struct
{
	Row rows[ROWS];
} Grid;

unsigned grid_strn(const Grid *grid, char *des, unsigned len)
{
	int x, y;
	unsigned count = 0;

	for(y = 0; y < ROWS; y++) {
		for(x = 0; x < COLUMNS; x++) {
			char str[16];
			unsigned c;
			
			c = sprintf(str, "%X ", grid->rows[y].values[x].value);

			if(count + c >= len) {
				des[count] = '\0';
				return count;
			}

			strcat(des, str);			
			count += c;
		}

		if(count >= len) {
			des[count] = '\0';
			return count;
		}

		strcat(des, "\n\0");
		count += 2;
	}

	des[count] = '\0';

	return count;
}

int main(int argc, char *argv[])
{
	int x, y, i;
	unsigned count;
	char buf[1024];
	Grid grid;
	Value *values;

	for(y = 0; y < ROWS; y++) {
		for(x = 0; x < COLUMNS; x++)
			grid.rows[y].values[x].value = (x + y * COLUMNS) % 0x10;
	}

	buf[0] = '\0';
	count = grid_strn(&grid, buf, sizeof(buf));

	printf("count: %d\n", count);
	printf("%s\n", buf);

	values = grid.rows->values;

	for(i = 0; i < LENGTH; i++)
		values[i].value = (values[i].value + 1) % 0x10;

	buf[0] = '\0';
	count = grid_strn(&grid, buf, sizeof(buf));

	printf("count: %d\n", count);
	printf("%s\n", buf);
	
	return 0;
}

