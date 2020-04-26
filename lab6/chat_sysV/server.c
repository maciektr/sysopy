#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>

#include <sys/msg.h>
#include <sys/ipc.h>
//- msgget, msgctl, msgsnd, msgrcv, ftok

#include "common.h"

void close_queue();
void stop_sig();
void init();

// Msg handlers 
void handle_msg(msg_t *buffer);
// CONNECT
int handle_connect(int first_id, int second_id);
// INIT
int register_client(int key, char *nick);
// STOP
void remove_client(int id);
// LIST
void send_list(int key);
// DISCONNECT
int set_free(int id);


int queue_id = -1;
int active_clients = 0;
client clients[CLIENTS_MAX];

int main() {
    init();
    msg_t buffer;
    while(1){
        int len;
        assert((len = msgrcv(queue_id, &buffer, MSG_T_LEN, MSG_TYPE_URGENT, QMOD)) != -1);
        handle_msg(&buffer);
    }
}

void handle_msg(msg_t *buffer){
    switch(buffer->mtype){
        case INIT:
            register_client(buffer->integer_msg, buffer->clients[0].nick);
            break;
        case LIST:
            send_list(buffer->sender_id);
            break;
        case CONNECT:
            assert(handle_connect(buffer->sender_id, buffer->integer_msg) >= 0);
            break;
        case DISCONNECT:
            set_free(buffer->sender_id);
            break;
        case STOP:
            remove_client(buffer->sender_id);
            break;
        case NONE:
            break;
    }
}

int handle_connect(int first_id, int second_id){
    printf("# Connect request from %d to %d\n", first_id, second_id);
    int first_key = -1, second_key = -1;
    int st_i = -1, nd_i = -1;
    char *first_nick = NULL;
    for(int i = 0; i < active_clients; i++){
        if(clients[i].id == first_id){
            first_key = clients[i].key;
            st_i = i;
            first_nick = clients[i].nick;
        }
        if(clients[i].id == second_id){
            second_key = clients[i].key;
            nd_i = i;
        }
    }

    if(first_key < 0)
        return -1;

    msg_t buffer;
    set_msg(&buffer, second_id, CONNECT, second_key < 0 ? -1:second_key);
    if(msgsnd(first_key, &buffer, MSG_T_LEN, QMOD) < 0)
        return -1;
    
    set_msg(&buffer, first_id, CONNECT, first_key);
    client cl;
    cl.id = first_id;
    strcpy(cl.nick, first_nick);
    buffer.clients[0]=cl;
    if(msgsnd(second_key, &buffer, MSG_T_LEN, QMOD) < 0)
        return -1;

    clients[st_i].status = BUSY;
    return 0;    
}

int register_client(int key, char *nick){
    if(active_clients >= CLIENTS_MAX){
        printf("# Register client request failed with too many clients.\n");
        msg_t buffer;
        set_msg(&buffer, 0, INIT, -1);
        assert(msgsnd(key, &buffer, MSG_T_LEN, QMOD | IPC_NOWAIT) != -1);
        return -1;
    }
    clients[active_clients].id = active_clients+1;
    printf("# Registering client (nick: %s, id: %d)\n", nick, clients[active_clients].id);
    clients[active_clients].key = key;
    clients[active_clients].status = FREE;
    strcpy(clients[active_clients].nick, nick);
    
    msg_t buffer;
    set_msg(&buffer, 0, INIT, clients[active_clients].id);
    active_clients++;

    assert(msgsnd(key, &buffer, MSG_T_LEN, QMOD | IPC_NOWAIT) != -1);
    return 0;
}

int set_free(int id){
    printf("# Client %d is now free for connections.\n", id);
    for(int i = 0; i < active_clients; i++){
        if(clients[i].id == id){
            clients[i].status = FREE;
            return 0;
        }
    }
    return -1;
}

void remove_client(int id){
    printf("# Client %d is removed from the server\n", id);
    if(active_clients <= 0)
        return;
    for(int i = 0; i<active_clients; i++)
        if(clients[i].id == id){
            active_clients--;
            clients[i] = clients[active_clients];
        }
}

void send_list(int id){
    printf("# Sending clients list to client %d.\n", id);
    msg_t buffer;

    int ac_i = 0;
    int key = -1;
    for(int i = 0; i<active_clients; i++){
        if(clients[i].id == id)
            key = clients[i].key;
        else if(clients[i].status == FREE)
            buffer.clients[ac_i++] = clients[i];
    }
    
    set_msg(&buffer, 0, LIST, ac_i);
    assert(key != -1);
    assert(msgsnd(key, &buffer, MSG_T_LEN, QMOD | IPC_NOWAIT) != -1);
}

void init(){
    atexit(close_queue);
    signal(SIGINT, stop_sig);

    queue_id = get_queue(get_homedir(), PROJECT_ID);
    assert(queue_id != -1);
    puts("# Server started!");
}

void stop_sig() {
    exit(EXIT_SUCCESS);
}

void close_queue() {
    puts("# Closing server queue.");
    if (queue_id == -1)
        return;
    assert(msgctl(queue_id, IPC_RMID, NULL) != -1);
}