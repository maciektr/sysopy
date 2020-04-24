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
client clients[MSG_T_CLIENTS_MAX];

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
        case LIST:
            send_list(buffer->sender_key);
            break;
        case CONNECT:

            break;
        case DISCONNECT:

            break;
        case STOP:

            break;
        case NONE:

            break;
    }
}

void send_list(int key){
    msg_t buffer;
    buffer.sender_key = queue_id;
    buffer.order = NONE;
    buffer.integer_msg = active_clients;
    for(int i = 0; i<active_clients; i++){
        buffer.clients[i] = clients[i];
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