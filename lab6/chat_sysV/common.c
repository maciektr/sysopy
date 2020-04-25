#include "common.h"


#include <stdio.h>
char *get_homedir() {
    char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    // printf("HOME: %s\n", homedir);
    assert(homedir != NULL);
    return homedir;
}

int get_queue(char *path, int id) {
    key_t key = ftok(path, id);
    assert(key != -1);
    // printf("KEY %d\n", key);
    return msgget(key, IPC_CREAT | QMOD);
}

void set_msg(msg_t *buffer, int sender_id, order_t order, int integer_msg){
    buffer->mtype = order;
    buffer->sender_id = sender_id;
    buffer->integer_msg = integer_msg;
}