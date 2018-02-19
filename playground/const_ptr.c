// 
// Filename: const_ptr.c
// Created: 2018-02-09 22:11:48 +0100
// Author: Felix Nared
//
// BUILD:
//   gcc const_ptr.c -o const_ptr.o
//
// RESULT:
//   mutate_pointer does not throw an error.
//   mutate_pointer warns if 'const *' is given as argument (discards 'const' qualifier)
//
//   mutate_const_array throws an error.
//   mutate_const_array throws an error even if it is not called.
//
//   mutate_array throws an error if 'const *' is given as argument.
//   
//


#include <stdio.h>


void mutate_const_pointer(const int *pointer)
{
	pointer = NULL;
}

void mutate_pointer(int *pointer)
{
	pointer = NULL;
}

void mutate_const_array(const int *array, unsigned length)
{
	int i;

	for(i = 0; i < length; i++)
		array[i] += 1;
}

void mutate_array(const int *array, unsigned length)
{
	int i;

	for(i = 0; i < length; i++)
		array[i] += 1;
}

int main(int argc, char *argv[])
{
	int nonconst[] = {1, 2, 3, 4, 5};
	const int isconst[] = {1, 2, 3, 4, 5};
	int i;

	int *nonc_ptr = nonconst;
	const int *isc_ptr = isconst;

	mutate_pointer(nonc_ptr);
	mutate_pointer(isc_ptr);

	printf("nonconst before mutation: ");
	for(i = 0; i < 5; i++)
		printf("%d ", nonconst[i]);
	putc('\n', stdout);

	printf("isconst before mutation: ");
	for(i = 0; i < 5; i++)
		printf("%d ", isconst[i]);
	putc('\n', stdout);	

	mutate_array(nonconst, 5);
	mutate_array(isconst, 5);

	printf("nonconst after mutation: ");
	for(i = 0; i < 5; i++)
		printf("%d ", nonconst[i]);
	putc('\n', stdout);

	printf("isconst after mutation: ");
	for(i = 0; i < 5; i++)
		printf("%d ", isconst[i]);
	putc('\n', stdout);
		       
	return 0;
}
