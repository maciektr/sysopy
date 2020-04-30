#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>


#include "common.h"

shm_t *shmaddr = NULL;
sem_t *acc_sem = NULL;
sem_t *ins_sem = NULL;
sem_t *pack_sem = NULL;
sem_t *send_sem = NULL;

void init();
void atexit_handler();
void sig_exit();

int main(){
    init();
    while(1){

    }
}

void init(){
    signal(SIGINT, sig_exit);

    srand(time(NULL));
    atexit(atexit_handler);

    shmaddr = get_shm();
    assert(shmaddr != NULL);

    acc_sem = get_lock(ACC_SEM, 1);
    ins_sem = get_lock(INS_SEM, ORDERS_N);
    pack_sem = get_lock(PACK_SEM, 0);
    send_sem = get_lock(SEND_SEM, 0);
}

void atexit_handler(){
    munmap(shmaddr, SHM_SIZE);
    shm_unlink(SHM_NAME);

    sem_close(acc_sem);
    sem_close(ins_sem);
    sem_close(pack_sem);
    sem_close(send_sem);
}

void sig_exit(){
    exit(EXIT_SUCCESS);
}