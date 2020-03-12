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

test_: 
	@echo "--------------------"
	@echo "Test dynamic:"
	@echo "--------------------"
	@echo "Small file, three differences:"
	time ./main.out create_table 1 compare_pairs a.txt:b.txt count_operations 0
	@echo "--------------------"
	@echo "Big files with a lot of differences:"
	time ./main.out create_table 1 compare_pairs big_text_a.txt:big_text_b.txt count_operations 0
	@echo "--------------------"
	@echo "Multiple comparisons, need reallocation of arrray."
	time ./main.out create_table 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt count_operations 0 count_operations 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt  remove_block 0 remove_block 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt remove_block 2 remove_block 3 remove_block 4 remove_block 5 remove_block 6 compare_pairs a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt remove_block 7 remove_block 8 remove_block 9 remove_block 10 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt
	@echo "--------------------"

test_dynamic: dynamic
	make test_

test_static: static
	make test_

test: test_dynamic test_static

optimization: static_lib
	for opt in 0 1 2 3  ; do \
		echo "====================" ; \
		echo "Optimized with" $$opt ; \
	    $(CC) -static -O$$opt main.c -L$(LIB_PATH) -l$(LIB) -o main.out ; \
		make test_ ; \
	done