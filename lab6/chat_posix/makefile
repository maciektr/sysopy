# Makefile

CC = gcc -std="c11" -O2 -D_XOPEN_SOURCE=700 -D_GNU_SOURCE

SERVER_FILE = server.c
SERVER_OUT = server.out

CLIENT_FILE = client.c
CLIENT_OUT = client.out

COMMON_FILE = common.c

main: clean
	$(CC) -D_NDEBUG $(SERVER_FILE) $(COMMON_FILE) -o $(SERVER_OUT) -lrt
	$(CC) -D_NDEBUG $(CLIENT_FILE) $(COMMON_FILE) -o $(CLIENT_OUT) -lrt

debug:
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(SERVER_FILE) $(COMMON_FILE) -o $(SERVER_OUT) -lrt
	$(CC) -fsanitize=address -Wall -Wextra -Wshadow -pedantic $(CLIENT_FILE) $(COMMON_FILE) -o $(CLIENT_OUT) -lrt

debug_fast:
	$(CC) -Wall -Wextra -Wshadow -pedantic $(SERVER_FILE) $(COMMON_FILE) -o $(SERVER_OUT) -lrt
	$(CC) -Wall -Wextra -Wshadow -pedantic $(CLIENT_FILE) $(COMMON_FILE) -o $(CLIENT_OUT) -lrt

clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT) *.o *.out