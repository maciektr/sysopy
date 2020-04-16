# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
CONS = cons.c
CONS_OUT = cons.out
MANF = manf.c
MANF_OUT = manf.out
TEST = test.c
TEST_OUT = test.out

main: clean
	$(CC) -static $(CONS) -o $(CONS_OUT)
	$(CC) -static $(MANF) -o $(MANF_OUT)
	$(CC) -static $(TEST) -o $(TEST_OUT)

debug: clean
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CONS) -o $(CONS_OUT)
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(MANF) -o $(MANF_OUT)
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(TEST) -o $(TEST_OUT)

clean:
	rm -f $(MANF_OUT) $(CONS_OUT) *.o *.out
