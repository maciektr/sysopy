# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = main.c
OUT = main.out

main: do_exec
	$(CC) -static -Wall -Wextra -Wshadow -pedantic $(FILE) -o $(OUT)

clean:
	rm -f $(OUT)

do_exec:
	$(CC) -static -Wall -Wextra -Wshadow -pedantic do_exec.c -o do_exec.o