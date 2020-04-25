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

void handle_msg(msg_t *buffer);
void close_queue();
void stop_sig();
void init();

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
            register_client(buffer->integer_msg);
            break;
        case LIST:
            send_list(buffer->integer_msg);
            break;
        case CONNECT:
            
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

int register_client(int key){
    if(active_clients >= CLIENTS_MAX)
        return -1;
    clients[active_clients].id = active_clients+1;
    clients[active_clients].key = key;
    clients[active_clients].status = FREE;
    
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

    queue_id = create_queue(get_homedir(), PROJECT_ID);
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