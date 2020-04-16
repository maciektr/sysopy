# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
NAME = main.c
NAME_OUT = main.out

main: clean
	$(CC) -static $(NAME) -o $(NAME_OUT)

clean:
	rm -f $(NAME_OUT) *.o *.out

test: main
	@./$(NAME_OUT) cmd.txt