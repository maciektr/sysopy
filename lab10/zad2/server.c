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
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>

#include "common.h"

#define PING_SLEEP 10
#define PING_TIMEOUT 5

client *clients[MAX_CLIENTS];
struct pollfd polls[MAX_CLIENTS+2];
int active_clients = 0;
board_t *boards[MAX_CLIENTS];
int active_boards = 0;
int client_waiting = -1;

int socket_unix_fd = -1;
int socket_inet_fd = -1;
char *socket_path = NULL;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t ping_thread = -1;
pthread_t server_thread = -1;

void run_server(char *path, int port);
void atexit_handle();
void *server_handle();
void *ping_handle();
void stop_sig();
void remove_client(int i);

int main(int argc, char *argv[]){
    srand(time(NULL));
    assert(argc == 3);

    int port = atoi(argv[1]);
    socket_path = argv[2];

    atexit(atexit_handle);
    signal(SIGINT, stop_sig);   

    for(int i = 0; i<MAX_CLIENTS; i++){
        clients[i] = (client *)malloc(sizeof(client));
        boards[i] = (board_t *)malloc(sizeof(board_t));
    }

    run_server(socket_path, port);

    assert(pthread_create(&ping_thread, NULL, ping_handle, NULL) >= 0);
    assert(pthread_create(&server_thread, NULL, server_handle, NULL) >= 0);

    assert(pthread_join(ping_thread, NULL) >= 0);
    assert(pthread_join(server_thread, NULL) >= 0);
}

void run_server(char *path, int port){
    struct sockaddr_un socket_unix;    

    socket_unix.sun_family = AF_UNIX;
    strcpy(socket_unix.sun_path, path);
    socket_unix_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    assert(socket_unix_fd >= 0);
    assert(bind(socket_unix_fd, (struct sockaddr *)&socket_unix, sizeof(socket_unix)) >= 0);
    // if(bind(socket_unix_fd, (struct sockaddr *)&socket_unix, sizeof(socket_unix)) < 0)
    //     perror("BIND");
    printf("Unix socket ready on: %s\n", path);

    struct sockaddr_in socket_inet;

    struct hostent* local_hostent = gethostbyname("localhost");
    struct in_addr address = *((struct in_addr*)(local_hostent->h_addr));

    socket_inet.sin_family = AF_INET;
    socket_inet.sin_port = htons(port);
    socket_inet.sin_addr.s_addr = address.s_addr;

    socket_inet_fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(socket_inet_fd  >= 0);
    assert(bind(socket_inet_fd, (struct sockaddr*) &socket_inet, sizeof(socket_inet)) >= 0);
    // if(bind(socket_inet_fd, (struct sockaddr*) &socket_inet, sizeof(socket_inet)) <0)
        // perror("bind2");
    printf("Inet socket ready on: %s:%d\n", inet_ntoa(address), port);
}

void atexit_handle() {
    msg_t msg;
    msg.type = CLOSE_GAME;
    strcpy(msg.body, "Serwer rozlaczony.");
    for(int i = 0; i<active_clients; i++)
        send_msg_with_addr(clients[i]->client_fd, &msg, clients[i]->addr, NULL);
        // send_msg(clients[i]->client_fd, &msg);

    assert(pthread_cancel(ping_thread) >= 0);
    assert(pthread_cancel(server_thread) >= 0);

    assert(close(socket_unix_fd) >= 0);
    assert(unlink(socket_path) >= 0);
    assert(close(socket_inet_fd) >= 0);

    for(int i = 0; i<MAX_CLIENTS; i++){
        free(clients[i]);
        free(boards[i]);
    }
}

void stop_sig(){
    exit(EXIT_SUCCESS);
}

void *ping_handle(){
    while(1){
        sleep(PING_SLEEP);
        puts("Pinging.");

        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i<active_clients; i++){
            clients[i]->ping_resp = 0;
            msg_t msg;
            msg.type = PING;
            msg.body[0] = '\0';
            send_msg_with_addr(clients[i]->client_fd, &msg, clients[i]->addr, NULL);
            // send_msg(clients[i]->client_fd, &msg);
        }
        pthread_mutex_unlock(&clients_mutex);
        
        sleep(PING_TIMEOUT);
        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i<active_clients; i++)
            if(clients[i]->ping_resp == 0){
                printf("Client %s timed out.\n", clients[i]->name);
                remove_client(i);
                i--;
            }
        pthread_mutex_unlock(&clients_mutex);
    }
}

void make_match(){
    if(client_waiting == -1){
        client_waiting = active_clients;
        return;
    }
    for(int k = 0; k < BOARD_N_CELLS; k++)
        boards[active_boards]->values[k] = FREE;
    clients[client_waiting]->board_id = active_boards;
    clients[active_clients]->board_id = active_boards;

    boards[active_boards]->players[0] = active_clients;
    boards[active_boards]->players[1] = client_waiting;

    msg_t msg;
    msg.type = START_GAME;
    
    strcpy(msg.body,"X");
    // send_msg(clients[active_clients]->client_fd, &msg);
    send_msg_with_addr(clients[active_clients]->client_fd, &msg, clients[active_clients]->addr, NULL);
    clients[active_clients]->mark = X;
    boards[active_boards]->move = 0;
    strcpy(msg.body,"O");
    // send_msg(clients[client_waiting]->client_fd, &msg);
    send_msg_with_addr(clients[client_waiting]->client_fd, &msg, clients[client_waiting]->addr, NULL);
    clients[client_waiting]->mark = O;

    active_boards++;
}

int get_client_id(char *name){
    for(int i = 0; i<active_clients; i++)
        if(strcmp(clients[i]->name, name))
            return i;
    return -1;
}

void register_client(msg_t *msg, struct sockaddr *addr, int fd, socklen_t len){
    if(get_client_id(msg->sender) != -1 || active_clients >= MAX_CLIENTS){
        msg_t m;
        m.type = REJ_INIT;
        m.body[0] = '\0';
        send_msg_with_addr(fd, &m, addr, &len);
    }
    strcpy(clients[active_clients]->name, msg->body);
    clients[active_clients]->board_id = -1;
    clients[active_clients]->ping_resp = 0;
    clients[active_clients]->client_fd = fd;
    clients[active_clients]->addr = addr;

    msg->type = ACC_INIT;
    msg->body[0] = '\0';
    send_msg_with_addr(fd, msg, addr, &len);
    make_match();

    printf("Client connected: %s\n", clients[active_clients]->name);
    active_clients++;
}

void remove_client(int i){
    if(clients[i]->board_id != -1){
        msg_t msg;
        msg.type = CLOSE_GAME;
        strcpy(msg.body, "Twoj przeciwnik rozlaczyl sie.");
        int opponent_id = boards[clients[i]->board_id]->players[0];
        if(opponent_id == i)
            opponent_id = boards[clients[i]->board_id]->players[1];
        send_msg_with_addr(clients[opponent_id]->client_fd, &msg, clients[opponent_id]->addr, NULL);
        boards[clients[i]->board_id] = boards[--active_boards];
    }
    clients[i] = clients[--active_clients];
    if(client_waiting == active_clients)
        client_waiting = i;
}

int won(board_t *board){
    for(int b_h = 0; b_h<BOARD_HEIGHT; b_h++)
        for(int b_w = 1; b_w < BOARD_WIDTH; b_w++){
            int cell_last = b_h * BOARD_WIDTH + b_w -1;
            int cell_id = b_h * BOARD_WIDTH + b_w;
            if(board->values[cell_id] != board->values[cell_last])  
                break;
            else if(board->values[cell_id] == FREE)
                break;
            else if(b_w == BOARD_WIDTH -1)
                return 1;
        }
    for(int b_w = 0; b_w < BOARD_WIDTH; b_w++)
        for(int b_h = 1; b_h<BOARD_HEIGHT; b_h++){
            int cell_last = (b_h - 1) * BOARD_WIDTH + b_w;
            int cell_id = b_h * BOARD_WIDTH + b_w;
            if(board->values[cell_id] != board->values[cell_last])  
                break;
            else if(board->values[cell_id] == FREE)
                break;
            else if(b_h == BOARD_HEIGHT -1)
                return 1;
        }
    for(int i = 1; i<BOARD_HEIGHT; i++){
        int cell_last = (i - 1) * BOARD_WIDTH + i - 1;
        int cell_id = i * BOARD_WIDTH + i;
        if(board->values[cell_id] != board->values[cell_last])  
            break;
        else if(board->values[cell_id] == FREE)
            break;
        else if(i == BOARD_WIDTH - 1)
            return 1;
    }
    int b_h = 1, b_w = BOARD_WIDTH - 2;
    while(b_w >= 0){
        int cell_last = (b_h - 1) * BOARD_WIDTH + (b_w + 1);
        int cell_id = b_h * BOARD_WIDTH + b_w;
        if(board->values[cell_id] != board->values[cell_last])  
            break;
        else if(board->values[cell_id] == FREE)
                break;
        else if(b_w == 0)
            return 1;
        b_w--;b_h++;
    }
    return 0;
}

int draw(board_t *board){
    for(int i = 0; i<BOARD_N_CELLS; i++)
        if(board->values[i] == FREE)
            return 0;
    return 1;
}

void sign_received(int client_id, int position){
    int board_id = clients[client_id]->board_id;
    int client_to_move = boards[board_id]->players[boards[board_id]->move];
    if(client_to_move != client_id)
        return;
    if(boards[board_id]->values[position] == FREE){
        boards[board_id]->values[position] = clients[client_id]->mark;
        int w = won(boards[board_id]);
        int d = draw(boards[board_id]);
        if(w == 1 || d == 1){
            msg_t msg;
            msg.type = CLOSE_GAME;
            int second_client = boards[board_id]->players[(boards[board_id]->move + 1) % 2];
            if(w == 1){
                strcpy(msg.body, "Wygrales!");
                // send_msg(clients[client_to_move]->client_fd, &msg);
                send_msg_with_addr(clients[client_to_move]->client_fd, &msg, clients[client_to_move]->addr, NULL);
                strcpy(msg.body, "Przegrales!");
            }else if(d == 1){
                strcpy(msg.body,"Remis!");
                // send_msg(clients[client_to_move]->client_fd, &msg);
                send_msg_with_addr(clients[client_to_move]->client_fd, &msg, clients[client_to_move]->addr, NULL);
            }
            // send_msg(clients[second_client]->client_fd, &msg); 
            send_msg_with_addr(clients[second_client]->client_fd, &msg, clients[second_client]->addr, NULL);
        }
        boards[board_id]->move = (boards[board_id]->move + 1) % 2;
    }

    msg_t msg;
    msg.type = SIGN_GAME;
    memcpy(msg.body, (char *)boards[board_id], sizeof(board_t));
    client_to_move = boards[board_id]->players[boards[board_id]->move];
    // send_msg(clients[client_to_move]->client_fd, &msg);
    send_msg_with_addr(clients[client_to_move]->client_fd, &msg, clients[client_to_move]->addr, NULL);
}


void *server_handle(){
    polls[1].fd = socket_unix_fd;
    polls[1].events = POLLIN;
    polls[0].fd = socket_inet_fd;
    polls[0].events = POLLIN;

    while(1){
        for(int i = 0; i < 2; i++) {
            polls[i].events = POLLIN;
            polls[i].revents = 0;
        }
        poll(polls, 2, -1);

        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i<2; i++){
            if(polls[i].revents & POLLIN){
                msg_t msg;
                struct sockaddr addr;
                socklen_t len = sizeof(&addr);
                read_msg_with_addr(polls[i].fd, &addr, &len, &msg);
                int id = -1;
                switch (msg.type)
                {
                case CONN_INIT:
                    register_client(&msg, &addr, polls[i].fd, len);
                    break;

                case SIGN_GAME: 
                    id = get_client_id(msg.sender);
                    sign_received(id, atoi(msg.body));
                    break;

                case CLOSE_GAME: 
                    id = get_client_id(msg.sender);
                    remove_client(id);
                    break;

                case PING: 
                    id = get_client_id(msg.sender);
                    clients[id]->ping_resp = 1;
                    break;
                
                default:
                    printf("Unrecognized message type: %s\n", msg.body);
                    break;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    pthread_exit((void *) 0);
}