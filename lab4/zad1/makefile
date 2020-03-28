# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = main.c
OUT = main.out

main: 
	$(CC) -static -Wall -Wextra -Wshadow -pedantic -D_NDEBUG $(FILE) -o $(OUT)

clean:
	rm -f $(OUT)