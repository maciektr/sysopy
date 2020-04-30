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
sem_t *send_sem = NULL;

int shm_rm(shm_t *shm);

void init();
void atexit_handler();
void sig_exit();

int main(){
    init();
    while(1){
        assert(sem_wait(acc_sem) != -1);
        assert(sem_wait(send_sem) != -1);

        int r = shm_rm(shmaddr);
        printf("(%d %s) Wysłałem zamówienie o wielkości: %d. Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n", (int)getpid(), get_timestamp(), r, n_to_pack(), n_to_send());

        assert(sem_post(acc_sem) != -1);
        assert(sem_post(ins_sem) != -1);
    }
}

int shm_rm(shm_t *shm){
    int res = shm->orders[shm->remove_index % ORDERS_N] * 3;
    shm->orders[shm->remove_index % ORDERS_N] = -1;
    shm->remove_index++;
    shm->remove_index %= ORDERS_N;
    return res;
}

void init(){
    signal(SIGINT, sig_exit);

    srand(time(NULL));
    atexit(atexit_handler);

    shmaddr = get_shm();
    assert(shmaddr != NULL);

    acc_sem = get_lock(ACC_SEM, 1);
    ins_sem = get_lock(INS_SEM, ORDERS_N);
    send_sem = get_lock(SEND_SEM, 0);
}

void atexit_handler(){
    munmap(shmaddr, SHM_SIZE);
    shm_unlink(SHM_NAME);

    sem_close(acc_sem);
    sem_unlink(ACC_SEM);

    sem_close(ins_sem);
    sem_unlink(INS_SEM);

    sem_close(send_sem);
    sem_unlink(SEND_SEM);
}

void sig_exit(){
    exit(EXIT_SUCCESS);
}