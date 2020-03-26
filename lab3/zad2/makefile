# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = matrix.c
OUT = matrix.out
PARENT_LIB = matrix_parent
WORKER_LIB = matrix_worker
INCLUDE_LIBS = -L. -l$(PARENT_LIB)  -l$(WORKER_LIB)

main: debug #standard

standard: lib
	$(CC) -static -D_NDEBUG $(FILE) $(INCLUDE_LIBS) -o $(OUT)

debug: lib
	$(CC) -Wall -fsanitize=address -Wshadow $(FILE) $(INCLUDE_LIBS) -o $(OUT)

debug_fast: lib
	$(CC) -Wall  -Wshadow $(FILE) $(INCLUDE_LIBS) -o $(OUT)

lib: clean
	$(CC) -c $(PARENT_LIB).c -o $(PARENT_LIB).o
	ar rcs lib$(PARENT_LIB).a $(PARENT_LIB).o
	$(CC) -c $(WORKER_LIB).c -o $(WORKER_LIB).o
	ar rcs lib$(WORKER_LIB).a $(WORKER_LIB).o

clean:
	rm -f $(OUT) $(WORKER_LIB).a $(PARENT_LIB).a *.o

# test: main 
# ./$(OUT) 