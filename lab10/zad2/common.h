#ifndef _COMMON_H
#define _COMMON_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#define _BSD_SOURCE

#define MAX_NAME_LEN 100
#ifndef UNIX_PATH_MAX 
#define UNIX_PATH_MAX 108
#endif // UNIX_PATH_MAX
#define MAX_MSG_LEN 4096
#define MAX_CLIENTS 100

typedef enum {LOCAL, NET} conn_mode_t;

typedef enum {
    NONE,
    PING, 
    CONN_INIT,
    ACC_INIT, 
    REJ_INIT, 
    WAIT_GAME,
    START_GAME,
    CLOSE_GAME,
    SIGN_GAME,
    MOVE_GAME

} msg_type_t;

typedef struct {
    msg_type_t type;
    char sender[MAX_NAME_LEN];
    char body[MAX_MSG_LEN];
} msg_t;

int send_msg(int socket_fd, msg_t *msg);
int read_msg(int socket_fd, msg_t *msg);
int read_msg_with_addr(int socket_fd, struct sockaddr* addr, socklen_t* addrlen, msg_t *msg);
int send_msg_with_addr(int socket_fd, msg_t *msg, struct sockaddr* addr, socklen_t *len);

typedef enum {X, O, FREE} cell_t;
#define BOARD_N_CELLS 9
#define BOARD_WIDTH 3
#define BOARD_HEIGHT 3
typedef struct {
    cell_t values[BOARD_N_CELLS];
    int players[2];
    int move;
} board_t;

typedef struct{
    char name[MAX_NAME_LEN];
    unsigned char ping_resp; 
    int client_fd;
    struct sockaddr* addr;
    int board_id;
    cell_t mark;
} client;

#endif //_COMMON_H