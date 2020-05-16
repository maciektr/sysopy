# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE

CODE = main.c
OUT = main.out

COMMON_FILE = common.c

main: clean
	$(CC) -D_NDEBUG $(CODE) -o $(OUT) -lpthread

debug:
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CODE) -o $(OUT) -lpthread

debug_fast:
	$(CC) -Wall -Wextra -Wshadow -pedantic $(CODE) -o $(OUT) -lpthread

clean:
	rm -f $(OUT) *.o *.out

test: main 
	./main.out 8 sign ./tests/baboon.pgm out.txt