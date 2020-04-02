# Makefile

CC = gcc -std="c11" -O0 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
SENDER = sender.c
SENDER_OUT = sender.out
CATCHER = catcher.c
CATCHER_OUT = catcher.out

main: catcher sender

clean:
	rm -f $(SENDER_OUT) $(CATCHER_OUT) *.o *.out

catcher: 
	$(CC) -static $(CATCHER) -o $(CATCHER_OUT)

sender: 
	$(CC) -static $(SENDER) -o $(SENDER_OUT)