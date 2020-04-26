#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>

#include <mqueue.h>
// - mq_open, mq_send, mq_receive, mq_getattr, mq_setattr, mq_close, mq_unlink, mq_notify

#include "common.h"
#define CMD_LEN 100

int cl_que = -1;
int srv_que = -1;
int my_id = -1;

void atexit_handle();
void stop_sig();
void get_id();
void init();

void print_help(int mode);

int main(){
    init();
    print_help(0);
    // clear_stdin();
    while(1){
        // check_queue();
        // printf("# ");
        // char cmd[CMD_LEN];
        // fgets(cmd, CMD_LEN, stdin);
        // if(cmd[0] == 10)
        //     continue;
        // remove_enter(cmd);
        // handle_cmd(cmd);
    }
}

void print_help(int mode){
    if(mode == 0){
        // not connected mode
        puts("Please insert one of the commands listed below:");
        puts("  - list - show all clients available.");
        puts("  - connect - connect to another client with.");
        puts("  - exit - shutdown the program.");
    }else if(mode == 1){
        // connected mode
        puts("You can insert one of the commands listed below. Do not forget the \"#\" prefix.");
        puts("  - #disconnect - to close your connection");
        puts("  - #exit - shutdown the program.");
    }
}


void stop_sig(){
    exit(EXIT_SUCCESS);
}