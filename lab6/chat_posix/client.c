#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
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

// Program functions
void print_help(int mode);
void check_queue();

// Command handlers
void handle_cmd(char *cmd);
void list_clients();
void handle_connect(int friend_id);

// Connected mode functions
void connected_mode(int key);
int handle_connected_cmd(char *cmd, int key);
void read_texts();

// List helper functions
void list_pretty_print(client clients[CLIENTS_MAX], int n);
void item_pretty_print(int id, char *nick);
int int_len(int n);

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
            printf("Another client (nick: %s, id: %d) is connecting to you.\n", buffer.msg.clients[0].nick, buffer.msg.clients[0].id);
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
        list_clients();                
    }else if(strcmp(cmd, "connect") == 0){
        handle_connect(-1);
    }else if(strcmp(cmd, "exit") == 0){
        exit(EXIT_SUCCESS);
    }else{
        print_help(0);
        return;
    }
}

void list_clients(){
    msg_buffer_t buffer;
    set_msg(&buffer.msg, my_id, LIST, 0);
 
    assert(mq_send(srv_que, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
    assert(mq_receive(cl_que, buffer.buffer, MSG_MAX_SIZE, NULL) != -1);

    list_pretty_print(buffer.msg.clients, buffer.msg.integer_msg);
}

void handle_connect(int friend_id){
    while(friend_id < 0){
        puts("Please insert id of the client you want to connect with.");
        scanf("%d", &friend_id);
    }
    msg_buffer_t buffer;
    set_msg(&buffer.msg, my_id, CONNECT, friend_id);
    assert(mq_send(srv_que, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
    assert(mq_receive(cl_que, buffer.buffer, MSG_MAX_SIZE, NULL) != -1);
    if(buffer.msg.integer_msg == -1){
        puts("Connection failed. Please try again.");
        return;
    }
    connected_mode(buffer.msg.integer_msg);
}

// Connected mode functions

void connected_mode(int key){
    assert(key >= 0);
    print_help(1);
    clear_stdin();

    while(1){
        read_texts();
        printf("> ");
        char txt[MSG_MAX_SIZE];
        fgets(txt, MSG_MAX_SIZE, stdin);
        if(txt[0] == 10)
            continue;
        remove_enter(txt);
        if(txt[0]=='#' && handle_connected_cmd((txt+1), key) != 0){
            print_help(0);
            return;
        }
        char buffer[MSG_MAX_SIZE];
        strcpy(buffer, txt);
        assert(mq_send(key, buffer, MSG_MAX_SIZE, 0) == 0);
    }
}


int handle_connected_cmd(char *cmd, int key){
    cut_word(cmd);
    if(strcmp(cmd, "exit") == 0){
        exit(EXIT_SUCCESS);
    } else if(strcmp(cmd, "disconnect") == 0){
        msg_buffer_t buffer;
        set_msg(&buffer.msg, my_id, DISCONNECT, 0);
        assert(mq_send(srv_que, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
        return DISCONNECT;
    }
    return 0;
}

void read_texts(){
    char buffer[MSG_MAX_SIZE];
    while(mq_receive(cl_que, buffer, MSG_MAX_SIZE, NULL) != -1)
        puts(buffer);
}

// List helper functions

void list_pretty_print(client clients[CLIENTS_MAX], int n){
    item_pretty_print(0,NULL);
    for(int i = 0; i < n; i++)
        item_pretty_print(clients[i].id, clients[i].nick);
    item_pretty_print(-1,NULL);
};  

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

int int_len(int n){
    int res = 0; 
    while(n){
        n/=10;
        res++;
    }
    return res;
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
        return (char)('A' + r - n_numb);
    return (char)('a' + r - n_numb - n_upper);
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
    strcpy(buffer.msg.sender_name, clq_name);

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