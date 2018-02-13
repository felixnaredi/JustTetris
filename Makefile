CC = gcc

main: main.c tetris.c math.c
	$(CC) -o $@.o $^ -lncurses

clean:
	rm *~ *.o
