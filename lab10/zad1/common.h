#ifndef _COMMON_H
#define _COMMON_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_NAME_LEN 100
#ifndef UNIX_PATH_MAX 
#define UNIX_PATH_MAX 108
#endif // UNIX_PATH_MAX
#define MAX_MSG_LEN 4096

typedef enum {LOCAL, NET} conn_mode_t;

typedef enum {
    CONN_INIT
} msg_type_t;

typedef struct {
    msg_type_t type;
    char body[MAX_MSG_LEN];
} msg_t;

int send_msg(int socket_fd, msg_t *msg);

typedef enum {X, O, FREE} cell_t;
#define BOARD_N_CELLS 9
#define BOARD_WIDTH 3
#define BOARD_HEIGHT 3
typedef struct {
    cell_t values[BOARD_N_CELLS];
} board_t;

#endif //_COMMON_H