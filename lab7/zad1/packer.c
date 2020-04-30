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
shm_t *shmaddr = NULL;

int shm_safe_pack(shm_t *shm);

void init();
void atexit_handler();
void sig_exit();

int main(){
    init();
    while(1){
        sem_qop(lock_id, -1);

        int r = shm_safe_pack(shmaddr);
        if(r >= 0)
            printf("(%d %s) Przygotowałem zamówienie o wielkości: %d. Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n", (int)getpid(), get_timestamp(), r, n_to_pack(shmaddr), n_to_send(shmaddr));

        sem_qop(lock_id, 1);
    }
}

int shm_safe_pack(shm_t *shm){
    if(n_to_pack(shm) > 0){
        shm->orders[shm->pack_index % ORDERS_N] *= 2;
        shm->pack_index++;
        shm->pack_index %= ORDERS_N;
        return 0;
    }
    return -1;
}


void init(){
    signal(SIGINT, sig_exit);

    srand(time(NULL));
    atexit(atexit_handler);

    shm_id = get_shm();
    assert(shm_id != -1);
    
    shmaddr = shmat(shm_id, NULL, 0);
    assert(shmaddr != NULL);

    lock_id = get_lock();
    assert(lock_id != -1);
}

void atexit_handler(){
    assert(semctl(lock_id, 0, IPC_RMID) == 0);
    assert(shmdt(shmaddr) == 0);
    shmctl(shm_id, IPC_RMID, NULL);
}

void sig_exit(){
    exit(EXIT_SUCCESS);
}