#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>

#include <sys/shm.h> 
#include <sys/ipc.h>
// shmget, shmclt, shmat, shmdt
// msgget, msgctl, msgsnd, msgrcv, ftok

#include "common.h"

int shm_id = -1;
int lock_id = -1;
void *shmaddr = NULL;

int main(){
    init();
    
}

void init(){
    atexit(atexit_handler);

    shm_id = get_shm();
    assert(shm_id != -1);
    
    shmaddr = shmat(shm_id,NULL,0);
    assert(shmaddr != NULL);

    lock_id = get_lock();
    assert(lock_id != -1);
}

void atexit_handler(){
    assert(shmdt(shmaddr) == 0);
    shmctl(shm_id, IPC_RMID, NULL);
}