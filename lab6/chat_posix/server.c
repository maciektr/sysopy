#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

#include <mqueue.h>
// - mq_open, mq_send, mq_receive, mq_getattr, mq_setattr, mq_close, mq_unlink, mq_notify

#include "common.h"

void atexit_handle();
void stop_sig();
void init();

void handle_msg(msg_t *msg);


mqd_t queue_id = -1;
int active_clients = 0;
client clients[CLIENTS_MAX];

int main(){
    init();
    while(1){
        msq_buffer_t buffer;
        assert(mq_receive(queue_id, buffer.buffer, MSG_MAX_SIZE, NULL) != -1);
        handle_msg(&buffer.msg);
    }
}

void handle_msg(msg_t *msg){
    switch(msg->order){
        case INIT:
            register_client(msg->integer_msg, msg->clients[0].nick);
            break;
        case LIST:
            // send_list(buffer->sender_id);
            break;
        case CONNECT:
            // assert(handle_connect(buffer->sender_id, buffer->integer_msg) >= 0);
            break;
        case DISCONNECT:
            // set_free(buffer->sender_id);
            break;
        case STOP:
            // remove_client(buffer->sender_id);
            break;
        default:
            break;
    }
}



void init(){
    atexit(atexit_handle);
    signal(SIGINT, stop_sig);

    queue_id = get_queue(SERVER_QUE_NAME, O_WRONLY);
    assert(queue_id != -1);  
    puts("# Server started!");
}

void stop_sig(){
    exit(EXIT_SUCCESS);
}

void atexit_handle(){
    puts("# Closing server queue.");
    if (queue_id == -1)
        return;
    assert(mq_close(queue_id) == 0);

    // for(int i = 0; i<active_clients; i++){
    //     msg_t buffer;
    //     set_msg(&buffer, 0, STOP, 0);
    //     assert(msgsnd(clients[i].key, &buffer, MSG_T_LEN, QMOD) != -1);
    // }
    assert(mq_unlink(SERVER_QUE_NAME) == 0);
}