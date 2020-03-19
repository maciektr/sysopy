# Makefile

CC = gcc -std="c11" -Wall

main: 
	$(CC) -static main.c -o main.out

debug: 
	$(CC) -fsanitize=address -Wshadow main.c -o main.out
	

