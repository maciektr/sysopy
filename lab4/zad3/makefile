# Makefile

CC = gcc -std="c11" -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = main.c
OUT = main.out

main: 
	$(CC) -static $(FILE) -o $(OUT)

clean:
	rm -f $(OUT)