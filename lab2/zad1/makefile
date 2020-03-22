# Makefile

CC = gcc -std="c11" -Wall -O3
FILE = sort.c

main: 
	$(CC) -static $(FILE) -o main.out

debug: 
	$(CC) -fsanitize=address -Wshadow $(FILE) -o main.out
	
test: main
	bash time.sh > wyniki.txt

clean:
	rm -f $(OUT)