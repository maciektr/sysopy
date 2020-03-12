# Makefile

CC = gcc -std="c11"
LIB = diffwrapper

main: static

debug: clean 
	$(CC) -Wall -Wshadow -c $(LIB).c -o $(LIB).o
	ar rcs lib$(LIB).a $(LIB).o

static: clean
	$(CC) -c $(LIB).c -o $(LIB).o
	ar rcs lib$(LIB).a $(LIB).o

dynamic: 
	$(CC) -fPIC -shared $(LIB).c -o lib$(LIB).so

clean:
	rm -f $(LIB).o lib$(LIB).a
	