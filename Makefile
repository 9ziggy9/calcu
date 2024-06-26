CC=cc
CFLAGS=-Wall -Wextra -pedantic -Wconversion -Wunreachable-code -Wno-gnu
EXE=./calcu

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o $(EXE) -lncurses

parse: main.c
	$(CC) $(CFLAGS) main.c -o $(EXE) -lncurses -lm -DPARSE_TEST

clean:
	rm -rf $(EXE) *.o *.i *.asm
