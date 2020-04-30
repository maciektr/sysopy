# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE

CODE1 = receiver.c
OUT1 = receiver.out

CODE2 = packer.c
OUT2 = packer.out

CODE3 = sender.c
OUT3 = sender.out

COMMON_FILE = common.c

main: clean
	$(CC) -D_NDEBUG $(CODE1) $(COMMON_FILE) -o $(OUT1) -lrt
	$(CC) -D_NDEBUG $(CODE2) $(COMMON_FILE) -o $(OUT2) -lrt
	$(CC) -D_NDEBUG $(CODE3) $(COMMON_FILE) -o $(OUT3) -lrt

debug:
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CODE1) $(COMMON_FILE) -o $(OUT1) -lrt
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CODE2) $(COMMON_FILE) -o $(OUT2) -lrt
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CODE3) $(COMMON_FILE) -o $(OUT3) -lrt

debug_fast:
	$(CC) -Wall -Wextra -Wshadow -pedantic $(CODE1) $(COMMON_FILE) -o $(OUT1) -lrt
	$(CC) -Wall -Wextra -Wshadow -pedantic $(CODE2) $(COMMON_FILE) -o $(OUT2) -lrt
	$(CC) -Wall -Wextra -Wshadow -pedantic $(CODE3) $(COMMON_FILE) -o $(OUT3) -lrt

clean:
	rm -f $(OUT1) $(OUT2) $(OUT3) *.o *.out