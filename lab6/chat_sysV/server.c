#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <pwd.h>

#include <sys/msg.h>
#include <sys/ipc.h>
//- msgget, msgctl, msgsnd, msgrcv, ftok

#include "common.h"
void close_queue();
void stop_sig();
int queue_id = -1;

int main(){
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    atexit(close_queue);
    signal(SIGINT,stop_sig);

    key_t key = ftok(homedir, PROJECT_ID);
    assert(key != -1);

    queue_id = msgget(key, IPC_CREAT);
    assert(queue_id != -1);

}

void stop_sig(){

}

void close_queue(){
    if(queue_id == -1)
        return;
    assert(msgctl(queue_id, IPC_RMID, NULL) != -1);
}