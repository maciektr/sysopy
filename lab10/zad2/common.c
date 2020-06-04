#include "common.h"

int send_msg(int socket_fd, msg_t *msg){
    return write(socket_fd, (void *)msg, sizeof(msg_t));
}

int read_msg(int socket_fd, msg_t *msg){
    return read(socket_fd, (void *)msg, sizeof(msg_t));
    // return recv(socket_fd, (void*)msg, sizeof(msg_t), 0);
} 

int read_msg_with_addr(int socket_fd, struct sockaddr* addr, socklen_t* addrlen, msg_t *msg){
    return recvfrom(socket_fd, (void*)msg, sizeof(msg_t), 0, addr, addrlen);
}

int send_msg_with_addr(int socket_fd, msg_t *msg, struct sockaddr* addr, socklen_t *len) {
    if(sendto(socket_fd, (void *)msg, sizeof(msg_t), 0, addr, len == NULL ? sizeof(struct sockaddr):*len) < 0)
        perror("sendto");
    return 0;
}