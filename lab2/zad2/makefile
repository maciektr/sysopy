# Makefile

CC = gcc -std="c11" -Wall -O2
FILE = find.c
OUT = find.out

main: 
	$(CC) -static $(FILE) -o $(OUT)

debug: 
	$(CC) -fsanitize=address -Wshadow $(FILE) -o $(OUT)
	