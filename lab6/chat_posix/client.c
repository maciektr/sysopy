#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#include <mqueue.h>
// - mq_open, mq_send, mq_receive, mq_getattr, mq_setattr, mq_close, mq_unlink, mq_notify

#include "common.h"
#define CMD_LEN 100

char clq_name[CLQ_NAME_LEN];
mqd_t cl_que = -1;
mqd_t srv_que = -1;
int my_id = -1;

void atexit_handle();
void stop_sig();
void get_id();
void init();

void print_help(int mode);
void handle_cmd(char *cmd);
void check_queue();

// Helper functions
void clear_stdin();
char random_alpha();
void get_clq_name(char *to, char *nick);
void remove_enter(char *txt);
void cut_word(char *cmd);

int main(){
    srand(time(NULL));
    init();
    print_help(0);
    clear_stdin();
    while(1){
        check_queue();
        printf("# ");
        char cmd[CMD_LEN];
        fgets(cmd, CMD_LEN, stdin);
        if(cmd[0] == 10)
            continue;
        remove_enter(cmd);
        handle_cmd(cmd);
    }
}

// Program functions

void check_queue(){
    msg_buffer_t buffer;
    // TODO: IPC_NOWAIT?
    if(mq_receive(cl_que, buffer.buffer, MSG_MAX_SIZE, NULL) != -1){
        switch (buffer.msg.order)
        {
        case CONNECT:
            printf("Another client (nick: %s, id: %d) is connecting to you.\n", buffer.clients[0].nick, buffer.clients[0].id);
            break;
        case STOP:
            puts("# Server is shutting down.");
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
        }
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

//Command handlers

void handle_cmd(char *cmd){
    cut_word(cmd);
    
    if (strcmp(cmd, "list") == 0){
        // list_clients();                
    }else if(strcmp(cmd, "connect") == 0){
        // handle_connect(-1);
    }else if(strcmp(cmd, "exit") == 0){
        exit(EXIT_SUCCESS);
    }else{
        print_help(0);
        return;
    }
}

// Helper functions

void cut_word(char *cmd){
    for(char *p = cmd; *p; p++)
        if(!isalpha(*p))
            *p = '\0';
        else
            *p = tolower(*p);
}

void remove_enter(char *txt){
    for(int i = 0; i<strlen(txt); i++)
        txt[i] = txt[i] == 10 ? ' ':txt[i];
}

void clear_stdin(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

char random_alpha(){
    int n_upper = ('Z' - 'A');
    int n_lower = ('z' - 'a');
    int n_numb = ('9' - '0');
    int N = n_numb + n_upper + n_lower;
    int r = rand() % N;
    if(r < n_numb)
        return (char)(r);
    if(r < n_numb + n_upper)
        return (char)('A' + r);
    return (char)('a' + r);
}

void get_clq_name(char *to, char *nick){
    to[0] = '/';
    to[1] = '\0';
    strcat(to, nick);
    int i = strlen(nick)+1;
    to[i++] = '_';
    while(i < CLQ_NAME_LEN)
        to[i++] = random_alpha();
}

// Client init

void init(){
    atexit(atexit_handle);
    signal(SIGINT, stop_sig);

    srv_que = get_queue(SERVER_QUE_NAME, O_WRONLY);
    assert(srv_que != -1);
    
    char nick[NICK_LEN];
    puts("Please insert your nick.");
    scanf("%s", nick);
    while(strlen(nick) > NICK_LEN){
        printf("Please insert valid nick. It can not be longer than %d\n", NICK_LEN);
        scanf("%s", nick);
    }

    get_clq_name(clq_name, nick);
    cl_que = get_queue(clq_name, O_RDONLY);
    assert(cl_que != -1);

    get_id(nick);
}

void get_id(char *nick){
    msg_buffer_t buffer;
    set_msg(&buffer.msg, 0, INIT, cl_que);

    client cl;
    assert(strlen(nick) < NICK_LEN);
    strcpy(cl.nick, nick);
    cl.key = cl_que;
    cl.id = -1;
    buffer.msg.clients[0] = cl;
    
    assert(mq_send(srv_que, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
    assert(mq_receive(cl_que, buffer.buffer, MSG_MAX_SIZE, NULL) != -1);
    my_id = buffer.msg.integer_msg;
    assert(my_id != -1);
}

void atexit_handle() {
    msg_buffer_t buffer;
    set_msg(&buffer.msg, my_id, STOP, 0);

    if(mq_send(srv_que, buffer.buffer, MSG_MAX_SIZE, 0) < 0)
        exit(EXIT_FAILURE);


    if (cl_que != -1){
        //TODO: Check if unlink possible?
        assert(mq_close(cl_que) == 0);
        assert(mq_unlink(clq_name) == 0);
    }
}

void stop_sig(){
    exit(EXIT_SUCCESS);
}