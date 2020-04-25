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

void list_pretty_print(client clients[CLIENTS_MAX], int n);
void item_pretty_print(int id, char *nick);
void handle_cmd(char *cmd);
void list_clients();
void print_help();


int main() {
    init();
    print_help();
    while(1){
        char cmd[CMD_LEN];
        scanf("%s", cmd);
        handle_cmd(cmd);
    }
}

void handle_cmd(char *cmd){
    for(char *p = cmd; *p; ++p) *p = tolower(*p);
    if (strcmp(cmd, "list") == 0){
        list_clients();                
    }else if(strcmp(cmd, "connect") == 0){

    }else if(strcmp(cmd, "exit") == 0){
        exit(EXIT_SUCCESS);
    }else{
        print_help();
        return;
    }
}

int int_len(int n){
    int res = 0; 
    while(n){
        n/=10;
        res++;
    }
    return res;
}

void item_pretty_print(int id, char *nick){
    int id_mlen = int_len(CLIENTS_MAX);
    if(id_mlen < 3)
        id_mlen = 3;
    int len = NICK_LEN + id_mlen +7;

    if(nick == NULL && id == 0){
        item_pretty_print(-1, NULL);
        char *txt = "| Free clients list:";
        printf("%s", txt);
        for(int i = 0 ; i<len - strlen(txt)-1; i++) printf(" ");
        printf("|\n|");
        for(int i = 0; i<len -2; i++) printf("-");
        printf("|\n| id:");
        for(int i = 0; i<id_mlen - 3; i++) printf(" ");
        printf(" | nick:");
        for(int i = 0; i<NICK_LEN - 5; i++) printf(" ");
        printf(" |\n");
    } else if(nick == NULL && id == -1){
        printf("|");
        for(int i = 0; i<len -2; i++) printf("-");
        printf("|\n");
    } else {
        printf("| %d",id);
        for(int i = 0; i < id_mlen - int_len(id); i++) printf(" ");
        printf(" | %s", nick);
        for(int i = 0; i < NICK_LEN - strlen(nick); i++) printf(" ");
        printf(" |\n");
    }
}

void list_pretty_print(client clients[CLIENTS_MAX], int n){
    item_pretty_print(0,NULL);
    for(int i = 0; i < n; i++)
        item_pretty_print(clients[i].id, clients[i].nick);
    item_pretty_print(-1,NULL);
};  

void list_clients(){
    msg_t buffer;
    set_msg(&buffer, my_id, LIST, 0);
 
    assert(msgsnd(srv_que, &buffer, MSG_T_LEN, QMOD) != -1);
    assert(msgrcv(cl_que, &buffer, MSG_T_LEN, MSG_TYPE_URGENT, QMOD) != -1);
    list_pretty_print(buffer.clients, buffer.integer_msg);

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

    cl_que = msgget(IPC_PRIVATE, QMOD);
    assert(cl_que != -1);

    srv_que = get_queue(get_homedir(), PROJECT_ID);
    assert(srv_que != -1);
    
    char nick[NICK_LEN];
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
    set_msg(&buffer, 0, INIT, cl_que);

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
    set_msg(&buffer, my_id, STOP, 0);

    if(msgsnd(srv_que, &buffer, MSG_T_LEN, QMOD) < 0)
        exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}