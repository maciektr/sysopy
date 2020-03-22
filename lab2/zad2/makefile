# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700
FILE = find.c
OUT = find.out

main:
	$(CC) -static -D_NDEBUG $(FILE) -o $(OUT)

debug: 
	$(CC) -Wall -fsanitize=address -Wshadow $(FILE) -o $(OUT)

debug_fast:
	$(CC) -Wall  -Wshadow $(FILE) -o $(OUT)