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

int queue_id = -1;

int main(){
    init();
    // msg_t buffer;
    while(1){
        // int len;
        // assert((len = msgrcv(queue_id, &buffer, MSG_T_LEN, MSG_TYPE_URGENT, QMOD)) != -1);
        // handle_msg(&buffer);
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