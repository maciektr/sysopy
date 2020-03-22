# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = find.c
OUT = find.out

main:
	$(CC) -static -D_NDEBUG $(FILE) -o $(OUT)

debug: 
	$(CC) -Wall -fsanitize=address -Wshadow $(FILE) -o $(OUT)

debug_fast:
	$(CC) -Wall  -Wshadow $(FILE) -o $(OUT)

make test: main
	find /etc -mtime 0  2> /dev/null | wc -l
	./$(OUT) -mtime 0 /etc | tail -n 1
	find /usr -atime -7 -maxdepth 2 2> /dev/null | wc -l
	./$(OUT) -atime -7 -maxdepth 2 /usr | tail -n 1