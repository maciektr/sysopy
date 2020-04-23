# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = main.c
OUT = main.out

main: clean
	$(CC) -static -D_NDEBUG $(FILE) -o $(OUT)

debug:
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(FILE) -o $(OUT)

debug_fast:
	$(CC) -Wall -Wextra -Wshadow -pedantic $(FILE) -o $(OUT)

clean:
	rm -f $(OUT) *.o *.out