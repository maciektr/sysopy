#ifndef _COMMON_H
#define _COMMON_H

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pwd.h>
#include <sys/msg.h>

#ifndef MSGMAX
#define MSGMAX 4056
#endif
#define PROJECT_ID 244
#define QMOD 0666
#define CLIENTS_MAX MSG_T_CLIENTS_MAX
#define NICK_LEN 15 

char *get_homedir();
int get_queue(char *path, int id);

typedef enum{FREE, BUSY} status_t;
typedef struct {
    int id; 
    int key;
    status_t status;
    char nick[NICK_LEN];
} client;

#define MSG_TYPE_URGENT (-1*(LONG_MAX-10))  
// Server-client message format
typedef enum {__, STOP, DISCONNECT, LIST, CONNECT, INIT, NONE} order_t;
#define MSG_T_CLIENTS_MAX (((MSGMAX - sizeof(int) - sizeof(order_t) - sizeof(long))/sizeof(client))-4)
typedef struct {
    long mtype;
    int sender_id;
    // order_t order
    long integer_msg;
    client clients[MSG_T_CLIENTS_MAX];
} message_t;
#define msg_t message_t
#define MSG_T_LEN (sizeof(msg_t) - sizeof(long))
void set_msg(msg_t *buffer, int sender_id, order_t order, int integer_msg);

// Client-client message format
#define TXTMSG_TEXT_MAX (MSGMAX - 0)
typedef struct {
    long mtype;
    char text[TXTMSG_TEXT_MAX];
} text_message_t;
#define txtmsg_t text_message_t
#define TXTMSG_T_LEN (sizeof(txtmsg_t) - sizeof(long))

#endif //_COMMON_H
