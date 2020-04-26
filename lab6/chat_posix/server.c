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
int register_client(char *name, char *nick);
void send_list(int id);
int handle_connect(int first_id, int second_id);
int set_free(int id);
void remove_client(int id);

mqd_t queue_id = -1;
int active_clients = 0;
client clients[CLIENTS_MAX];

int main(){
    init();
    while(1){
        msg_buffer_t buffer;
        assert(mq_receive(queue_id, buffer.buffer, MSG_MAX_SIZE, NULL) != -1);
        handle_msg(&buffer.msg);
    }
}

void handle_msg(msg_t *msg){
    switch(msg->order){
        case INIT:
            register_client(msg->clients[0].que_name, msg->clients[0].nick);
            break;
        case LIST:
            send_list(msg->sender_id);
            break;
        case CONNECT:
            assert(handle_connect(msg->sender_id, msg->integer_msg) >= 0);
            break;
        case DISCONNECT:
            set_free(msg->sender_id);
            break;
        case STOP:
            remove_client(msg->sender_id);
            break;
        default:
            break;
    }
}

int register_client(char *name, char *nick){
    if(active_clients >= CLIENTS_MAX){
        printf("# Register client request failed with too many clients.\n");
        msg_buffer_t buffer;
        set_msg(&buffer.msg, 0, INIT, -1);
        assert(mq_send(get_queue(name, O_WRONLY), buffer.buffer, MSG_MAX_SIZE, 0) == 0);
        return -1;
    }
    clients[active_clients].id = active_clients+1;
    printf("# Registering client (nick: %s, id: %d)\n", nick, clients[active_clients].id);
    clients[active_clients].key = get_queue(name, O_WRONLY);
    clients[active_clients].status = FREE;
    strcpy(clients[active_clients].que_name, name);
    strcpy(clients[active_clients].nick, nick);
    
    msg_buffer_t buffer;
    set_msg(&buffer.msg, 0, INIT, clients[active_clients].id);
    assert(mq_send(clients[active_clients].key, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
    active_clients++;
    return 0;
}

void send_list(int id){
    printf("# Sending clients list to client %d.\n", id);
    msg_buffer_t buffer;

    int ac_i = 0;
    int key = -1;
    for(int i = 0; i<active_clients; i++){
        if(clients[i].id == id)
            key = clients[i].key;
        else if(clients[i].status == FREE)
            buffer.msg.clients[ac_i++] = clients[i];
    }
    
    set_msg(&buffer.msg, 0, LIST, ac_i);
    assert(key != -1);
    assert(mq_send(key, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
}

int handle_connect(int first_id, int second_id){
    printf("# Connect request from %d to %d\n", first_id, second_id);
    int first_key = -1, second_key = -1;
    int st_i = -1, nd_i = -1;
    char *first_nick = NULL;
    char *second_que = NULL;
    
    for(int i = 0; i < active_clients; i++){
        if(clients[i].id == first_id){
            first_key = clients[i].key;
            st_i = i;
            first_nick = clients[i].nick;
        }
        if(clients[i].id == second_id){
            second_key = clients[i].key;
            nd_i = i;
            second_que = clients[i].que_name;
        }
    }

    if(first_key < 0)
        return -1;

    msg_buffer_t buffer;    
    client cl;
    cl.id = second_id;
    strcpy(cl.que_name, second_que);
    buffer.msg.clients[0]=cl;
    set_msg(&buffer.msg, second_id, CONNECT, second_key >= 0 ? 0:-1);

    if(mq_send(first_key, buffer.buffer, MSG_MAX_SIZE, 0) < 0)
        return -1;
    
    set_msg(&buffer.msg, first_id, CONNECT, first_key);
    cl.id = first_id;
    strcpy(cl.nick, first_nick);
    buffer.msg.clients[0]=cl;
    if(mq_send(second_key, buffer.buffer, MSG_MAX_SIZE, 0) < 0)
        return -1;

    clients[st_i].status = BUSY;
    return 0;    
}

int set_free(int id){
    printf("# Client %d is now free for connections.\n", id);
    for(int i = 0; i < active_clients; i++){
        if(clients[i].id == id){
            clients[i].status = FREE;
            return 0;
        }
    }
    return -1;
}

void remove_client(int id){
    printf("# Client %d is removed from the server\n", id);
    if(active_clients <= 0)
        return;
    for(int i = 0; i<active_clients; i++)
        if(clients[i].id == id){
            active_clients--;
            clients[i] = clients[active_clients];
        }
}

void init(){
    atexit(atexit_handle);
    signal(SIGINT, stop_sig);

    queue_id = get_queue(SERVER_QUE_NAME, O_RDONLY);
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

    for(int i = 0; i<active_clients; i++){
        msg_buffer_t buffer;
        set_msg(&buffer.msg, 0, STOP, 0);
        assert(mq_send(clients[i].key, buffer.buffer, MSG_MAX_SIZE, 0) == 0);
    }
    //TODO: Check if unlink possible?
    assert(mq_unlink(SERVER_QUE_NAME) == 0);
}