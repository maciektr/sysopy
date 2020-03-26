# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = matrix.c
OUT = matrix.out

main: debug
	# $(CC) -static -D_NDEBUG $(FILE) -o $(OUT)

debug: 
	$(CC) -Wall -fsanitize=address -Wshadow $(FILE) -o $(OUT)

debug_fast:
	$(CC) -Wall  -Wshadow $(FILE) -o $(OUT)

clean:
	rm -f $(OUT)

test: main 
	./$(OUT) 