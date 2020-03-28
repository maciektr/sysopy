# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
FILE = matrix.c
OUT = matrix.out
PARENT_LIB = matrix_parent
WORKER_LIB = matrix_worker
INCLUDE_LIBS = -L. -l$(PARENT_LIB)  -l$(WORKER_LIB)
GEN_FILE = gen_matrix.c
GEN_OUT = gen.o


main: debug #standard

standard: lib
	$(CC)  -static -D_NDEBUG $(FILE) $(INCLUDE_LIBS) -o $(OUT)

debug: lib
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow $(FILE) $(INCLUDE_LIBS) -o $(OUT)

debug_fast: lib
	$(CC) -Wall -Wextra -Wshadow $(FILE) $(INCLUDE_LIBS) -o $(OUT)

lib: clean
	$(CC) -c $(PARENT_LIB).c -o $(PARENT_LIB).o
	ar rcs lib$(PARENT_LIB).a $(PARENT_LIB).o
	$(CC) -c $(WORKER_LIB).c -o $(WORKER_LIB).o
	ar rcs lib$(WORKER_LIB).a $(WORKER_LIB).o

clean:
	rm -f $(OUT) $(GEN_OUT) lib$(WORKER_LIB).a lib$(PARENT_LIB).a *.o *.a
	rm -rf .runtime

gen: gen_debug
	# $(CC) -static -D_NDEBUG $(GEN_FILE) -o $(GEN_OUT)

gen_debug:
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(GEN_FILE) -o $(GEN_OUT)

# test: main 
# ./$(OUT) 