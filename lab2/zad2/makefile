# Makefile

CC = gcc -std="c11" -Wall -O2
FILE = find.c
OUT = find.out

main: 
	$(CC) -static $(FILE) -D_XOPEN_SOURCE=500 -o $(OUT)

debug: 
	$(CC) -fsanitize=address -D_XOPEN_SOURCE=500 -Wshadow $(FILE) -o $(OUT)
	