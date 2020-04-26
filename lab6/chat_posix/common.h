#ifndef _COMMON_H
#define _COMMON_H

#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>

#define SERVER_QUE_NAME "/chat_server_que"
#define QMOD (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define MSG_MAX_N 10
#define MSG_MAX_SIZE 8192
#define NICK_LEN 15 

int get_queue(char *name, int mode);

typedef enum{FREE, BUSY} status_t;
typedef struct {
    int id; 
    int key;
    status_t status;
    char nick[NICK_LEN];
} client;


typedef enum {__, STOP, DISCONNECT, LIST, CONNECT, INIT, NONE} order_t;

#endif //_COMMON_H
