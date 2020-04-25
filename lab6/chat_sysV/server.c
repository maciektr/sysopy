#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
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
    switch(buffer->order){
        case INIT:
            register_client(buffer->integer_msg, buffer->clients[0].nick);
            break;
        case LIST:
            send_list(buffer->integer_msg);
            break;
        case CONNECT:
            handle_connect(buffer->sender_id, buffer->integer_msg);
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
    int first_key = -1, second_key = -1;
    int st_i = -1, nd_i = -1;
    for(int i = 0; i < active_clients; i++){
        if((clients[i].id == first_id || clients[i].id == second_id) && clients[i].status != FREE)
            return -1;
        if(clients[i].id == first_id){
            first_key = clients[i].key;
            st_i = i;
        }
        if(clients[i].id == second_id){
            second_key = clients[i].key;
            nd_i = i;
        }
    }
    if(first_key < 0 || second_key < 0)
        return -1;

    msg_t buffer;
    buffer.sender_id = first_id;
    buffer.order = CONNECT;
    buffer.integer_msg = first_key;

    if(msgsnd(second_key, &buffer, MSG_T_LEN, QMOD) < 0)
        return -1;
    
    buffer.sender_id = second_id;
    buffer.order = CONNECT;
    buffer.integer_msg = second_key;

    if(msgsnd(first_key, &buffer, MSG_T_LEN, QMOD) < 0)
        return -1;

    clients[st_i].status = BUSY;
    clients[nd_i].status = BUSY;
    return 0;    
}

int register_client(int key, char *nick){
    if(active_clients >= CLIENTS_MAX)
        return -1;
    clients[active_clients].id = active_clients+1;
    clients[active_clients].key = key;
    clients[active_clients].status = FREE;
    strcpy(clients[active_clients].nick, nick);
    
    msg_t buffer;
    buffer.sender_id = 0;
    buffer.order = INIT;
    buffer.integer_msg = clients[active_clients].id;
    active_clients++;

    return msgsnd(key, &buffer, MSG_T_LEN, QMOD);
}

int set_free(int id){
    for(int i = 0; i < active_clients; i++){
        if(clients[i].id == id){
            clients[i].status = FREE;
            return 0;
        }
    }
    return -1;
}

void remove_client(int id){
    if(active_clients <= 0)
        return;
    for(int i = 0; i<active_clients; i++)
        if(clients[i].id == id){
            active_clients--;
            clients[i] = clients[active_clients];
        }
}

void send_list(int key){
    msg_t buffer;
    buffer.sender_id = 0;
    buffer.order = NONE;
    buffer.integer_msg = active_clients;
    int ac_i = 0;
    for(int i = 0; i<active_clients; i++){
        if(clients[i].status == FREE)
            buffer.clients[ac_i++] = clients[i];
    }
    msgsnd(key, &buffer, MSG_T_LEN, QMOD);
}

void init(){
    atexit(close_queue);
    signal(SIGINT, stop_sig);

    queue_id = get_queue(get_homedir(), PROJECT_ID);
    assert(queue_id != -1);
}

void stop_sig() {
    exit(EXIT_SUCCESS);
}

void close_queue() {
    if (queue_id == -1)
        return;
    assert(msgctl(queue_id, IPC_RMID, NULL) != -1);
}