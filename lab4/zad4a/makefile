# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
SENDER = sender.c
SENDER_OUT = sender.out
CATCHER = catcher.c
CATCHER_OUT = catcher.out

main: catcher sender

clean:
	rm -f $(SENDER_OUT) $(CATCHER_OUT) *.o *.out

catcher: 
	$(CC) -static -Wall -Wextra -Wshadow -pedantic $(CATCHER) -o $(CATCHER_OUT)

sender: 
	$(CC) -static -Wall -Wextra -Wshadow -pedantic $(SENDER) -o $(SENDER_OUT)