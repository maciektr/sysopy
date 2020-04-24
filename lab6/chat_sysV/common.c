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

int create_queue(char *path, int id) {
    key_t key = ftok(path, id);
    assert(key != -1);
    // printf("KEY %d\n", key);
    return msgget(key, IPC_CREAT | QMOD);
}