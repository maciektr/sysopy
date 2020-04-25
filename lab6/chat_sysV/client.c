#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>

#include <sys/msg.h>
#include <sys/ipc.h>
//- msgget, msgctl, msgsnd, msgrcv, ftok

#include "common.h"
#define CMD_LEN 100

int cl_que = -1;
int srv_que = -1;
int my_id = -1;

void close_queue();
void stop_sig();
void get_id();
void init();

int main() {
    init();
    print_help();
    while(1){
        char cmd[CMD_LEN];
        scanf("%s\n", cmd);
        handle_cmd(cmd);
    }
}

void handle_cmd(char *cmd){
    for(char *p = cmd; *p; ++p) *p = tolower(*p);
    
    if (strcmp(cmd, "list") == 0){
        
    }else if(strcmp(cmd, "connect") == 0){

    }else if(strcmp(cmd, "exit") == 0){
        exit(EXIT_SUCCESS);
    }else{
        print_help();
        return;
    }
}

void print_help(){
    puts("Please insert one of the commands listed below:");
    puts("  - list - show all clients available.");
    puts("  - connect %id - connect to client with id \"%id\"");
    puts("  - exit - to shutdown the program.");
}

void init(){
    atexit(close_queue);
    signal(SIGINT, stop_sig);

    cl_que = get_queue(get_homedir(), IPC_PRIVATE);
    assert(cl_que != -1);

    srv_que = get_queue(get_homedir(), PROJECT_ID);
    assert(srv_que != -1);
    
    char *nick = NULL;
    puts("Please insert your nick.");
    scanf("%s", nick);
    while(strlen(nick) > NICK_LEN){
        printf("Please insert valid nick. It can not be longer than %d\n", NICK_LEN);
        scanf("%s", nick);
    }
    get_id(nick);
}

void get_id(char *nick){
    msg_t buffer;
    buffer.sender_id = 0;
    buffer.order = INIT;
    buffer.integer_msg = cl_que;

    client cl;
    assert(strlen(nick) < NICK_LEN);
    strcpy(cl.nick, nick);
    cl.key = cl_que;
    cl.id = -1;
    buffer.clients[0] = cl;
    
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