#include "common.h"

int send_msg(int socket_fd, msg_t *msg){
    return write(socket_fd, (void *)msg, sizeof(msg_t));
}

int read_msg(int socket_fd, msg_t *msg){
    return read(socket_fd, (void *)msg, sizeof(msg_t));
    // return recv(socket_fd, (void*)msg, sizeof(msg_t), 0);
}