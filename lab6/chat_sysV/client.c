#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>

#include <sys/msg.h>
#include <sys/ipc.h>
//- msgget, msgctl, msgsnd, msgrcv, ftok

#include "common.h"
int cl_que = -1;
int srv_que = -1;
int my_id = -1;

void close_queue();
void stop_sig();
void get_id();
void init();

int main() {
    init();

}

void init(){
    atexit(close_queue);
    signal(SIGINT, stop_sig);

    cl_que = get_queue(get_homedir(), IPC_PRIVATE);
    assert(cl_que != -1);

    srv_que = get_queue(get_homedir(), PROJECT_ID);
    assert(srv_que != -1);

    get_id();
}

void get_id(){
    msg_t buffer;
    buffer.sender_id = 0;
    buffer.order = INIT;
    buffer.integer_msg = cl_que;
    assert(msgsnd(srv_que, &buffer, MSG_T_LEN, QMOD) != -1);
    assert(msgrcv(cl_que, &buffer, MSG_T_LEN, MSG_TYPE_URGENT, QMOD) != -1);
    my_id = buffer.integer_msg;
    assert(my_id != -1);

}

void close_queue() {
    if (cl_que != -1)
        assert(msgctl(cl_que, IPC_RMID, NULL) != -1);
}

void stop_sig(){
    msg_t buffer;
    buffer.sender_id = my_id;
    buffer.order = STOP;
    buffer.integer_msg = 0;

    if(msgsnd(srv_que, &buffer, MSG_T_LEN, QMOD) < 0)
        exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}