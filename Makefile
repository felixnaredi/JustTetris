CC = gcc

main: main.c interface.c tetris.c math.c
	$(CC) -o $@.o $^ -lncurses

main-debug: main.c interface.c tetris.c math.c debug.c
	$(CC) -o $@.o $^ -DJS_DEBUG -lncurses

clean:
	rm *~ *.o
