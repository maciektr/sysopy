# Makefile

CC = gcc -std="c11"
LIB_PATH = ../Z1/
LIB = diffwrapper

main: 
	make static

debug: dynamic_lib
	$(CC) -fsanitize=address -Wall -Wshadow main.c -L. -Wl,-rpath=. -l$(LIB) -o main.out
	
static_lib: 
	make -C $(LIB_PATH) static
	cp $(LIB_PATH)$(LIB).o $(LIB).o

static: static_lib
	$(CC) -static main.c -L$(LIB_PATH) -l$(LIB) -o main.out

dynamic_lib: 
	make -C $(LIB_PATH) dynamic
	cp $(LIB_PATH)lib$(LIB).so lib$(LIB).so

dynamic: dynamic_lib
	$(CC) main.c -L. -Wl,-rpath=. -l$(LIB) -o main.out

	