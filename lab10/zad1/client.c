#include <linux/limits.h>
#include <sys/resource.h>
#include <stdatomic.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/time.h>
#include <stdbool.h> 
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <time.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>

#include "common.h"

int get_connection(conn_mode_t mode, char *name, char *address);
void read_args(int argc, char *argv[], char *name, char *server_ip, conn_mode_t *connection_mode);
void atexit_handle();
void stop_sig();

int main(int argc, char *argv[]){
    char *name, *address;
    conn_mode_t connection_mode;
    read_args(argc, argv, name, address, &connection_mode);
    
    atexit(atexit_handle);
    signal(SIGINT, stop_sig);

    int server_fd = get_connection(connection_mode, name, address);
    assert(server_fd != -1);

    
}

int get_connection(conn_mode_t mode, char *name, char *address){
    int socket_fd = -1;
    if(mode == LOCAL){
        struct sockaddr_un socket_addr;
        socket_addr.sun_family = AF_UNIX;
        strcpy(socket_addr.sun_path, address);
        int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        assert(socket_fd >= 0);
        assert(connect(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr))>=0);
    }else if(mode == NET){
        struct sockaddr_in socket_addr;
        socket_addr.sin_family = AF_INET;

        char *colon_pointer = strchr(address, ':');
        assert(colon_pointer != NULL);
        int port_n = atoi(colon_pointer + 1);
        *colon_pointer = '\0';
        socket_addr.sin_port = htons(port_n);
        socket_addr.sin_addr.s_addr = inet_addr(address);

        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(socket_fd >= 0);
        assert(connect(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr))>=0);
    }

    msg_t msg;
    msg.type = CONN_INIT;
    strcpy(msg.body, name);
    send_msg(socket_fd, &msg);
    return socket_fd;
}

void read_args(int argc, char *argv[], char *name, char *address, conn_mode_t *connection_mode){
    assert(argc == 4);

    name = argv[1];
    assert(strlen(name) <= MAX_NAME_LEN);
    char *con_m = argv[2];
    address = argv[3];
    
    if(strcmp(con_m, "local") == 0 || strcmp(con_m, "LOCAL") == 0)
        *connection_mode = LOCAL;
    else if(strcmp(con_m, "net") == 0 || strcmp(con_m, "NET") == 0)
        *connection_mode = NET;
    else{
        puts("Bledny tryb pracy!");
        exit(EXIT_FAILURE);
    }

    if(*connection_mode == LOCAL)
        assert(strlen(address) <= UNIX_PATH_MAX);
}

void atexit_handle() {

}

void stop_sig(){
    exit(EXIT_SUCCESS);
}