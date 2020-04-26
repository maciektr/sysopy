#ifndef _COMMON_H
#define _COMMON_H

#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>

#define SERVER_QUE_NAME "/chat_server_que"
#define QMOD (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define CLIENTS_MAX MSG_T_CLIENTS_MAX
#define MSG_MAX_SIZE 8192
#define MSG_MAX_N 10
#define NICK_LEN 15 

int get_queue(char *name, int mode);

typedef enum{FREE, BUSY} status_t;
typedef struct {
    int id; 
    mqd_t key;
    status_t status;
    char nick[NICK_LEN];
} client;


typedef enum {__, STOP, DISCONNECT, LIST, CONNECT, INIT, NONE} order_t;
#define MSG_T_CLIENTS_MAX (((MSG_MAX_SIZE - sizeof(int) - sizeof(order_t) - sizeof(long))/sizeof(client))-4)
typedef struct {
    order_t order;
    int sender_id;
    long integer_msg;
    client clients[MSG_T_CLIENTS_MAX];
} message_t;
typedef union {
    char buffer[MSG_MAX_SIZE];
    message_t msg;
} msq_buffer_t;
#define msq_t message_t

#endif //_COMMON_H
