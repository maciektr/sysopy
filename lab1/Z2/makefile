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

test_dynamic: dynamic
	@echo "--------------------"
	@echo "Test dynamic:"
	@echo "--------------------"
	@echo "Small file, three differences:"
	./main.out create_table 1 compare_pairs a.txt:b.txt count_operations 0
	@echo "--------------------"
	@echo "Big files with a lot of differences:"
	./main.out create_table 1 compare_pairs big_text_a.txt:big_text_b.txt count_operations 0
	@echo "--------------------"
	@echo "Multiple comparisons, need reallocation of arrray."
	./main.out create_table 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt count_operations 0 count_operations 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt
	@echo "--------------------"

test_static: static
	@echo "--------------------"
	@echo "Test static:"
	@echo "--------------------"
	@echo "Small file, three differences:"
	./main.out create_table 1 compare_pairs a.txt:b.txt count_operations 0
	@echo "--------------------"
	@echo "Big files with a lot of differences:"
	./main.out create_table 1 compare_pairs big_text_a.txt:big_text_b.txt count_operations 0
	@echo "--------------------"
	@echo "Multiple comparisons, need reallocation of arrray."
	./main.out create_table 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt count_operations 0 count_operations 1 compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt compare_pairs big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt big_text_a.txt:big_text_b.txt a.txt:b.txt
	@echo "--------------------"

test: test_dynamic test_static