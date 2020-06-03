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

#define PING_SLEEP 10
#define PING_TIMEOUT 5

client *clients[MAX_CLIENTS];
int active_clients = 0;

int main(int argc, char *argv[]){
    srand(time(NULL));
    assert(argc == 3);

    int port = atoi(argv[1]);
    char *socket_path = argv[2];

    atexit(atexit_handle);
    signal(SIGINT, stop_sig);   

    
}

void run_server(char *path, int port){
    struct sockaddr_un socket_unix;    
    int socket_unix_fd = -1;

    socket_unix.sun_family = AF_UNIX;
    strcpy(socket_unix.sun_path, path);
    socket_unix_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    assert(socket_unix_fd >= 0);
    assert(bind(socket_unix_fd, (struct sockaddr *)&socket_unix, sizeof(socket_unix)) >= 0);
    assert(listen(socket_unix_fd, MAX_CLIENTS) >= 0);
    printf("Unix socket ready on: %s\n", path);

    struct sockaddr_in socket_inet;
    int socket_inet_fd = -1;

    struct hostent* local_hostent = gethostbyname("localhost");
    struct in_addr address = *(struct in_addr*) local_hostent->h_addr;

    socket_inet.sin_family = AF_INET;
    socket_inet.sin_port = htons(port);
    socket_inet.sin_addr.s_addr = address.s_addr;

    socket_inet_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(socket_inet_fd  >= 0);
    assert(bind(socket_inet_fd, (struct sockaddr*) &socket_inet, sizeof(socket_inet)) >= 0);
    assert(listen(socket_inet_fd, MAX_CLIENTS) >= 0);
    printf("INET socket ready on: %s:%d\n", inet_ntoa(address), port);
}

void atexit_handle() {
    msg_t msg;
    msg.type = CLOSE_GAME;
    strcpy(msg.body, "Serwer rozlaczony.");
    // for(int i = 0; i<active_clients; i++)
        // send_msg(clients[i]., &msg);

    //SHUTDOWN
}

void stop_sig(){
    exit(EXIT_SUCCESS);
}