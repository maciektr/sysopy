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

void shm_ins(shm_t *shm, int val);

void init();
void atexit_handler();
void sig_exit();

int main(){
    init();
    while(1){
        int val = rand() % 1000;
        assert(sem_wait(acc_sem) != -1);
        assert(sem_wait(ins_sem) != -1);

        shm_ins(shmaddr, val);
        printf("(%d %s) Dodałem liczbę: %d. Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d.\n", (int)getpid(), get_timestamp(), val, n_to_pack(), n_to_send());

        assert(sem_post(acc_sem) != -1);
        assert(sem_post(pack_sem) != -1);
    }
}

void shm_ins(shm_t *shm, int val){
    shm->orders[shm->insert_index % ORDERS_N] = val;
    shm->insert_index++;
    shm->insert_index %= ORDERS_N;
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
}

void atexit_handler(){
    assert(munmap(shmaddr, SHM_SIZE) == 0);
    assert(shm_unlink(SHM_NAME) == 0);

    sem_close(acc_sem);
    sem_unlink(ACC_SEM);

    sem_close(ins_sem);
    sem_unlink(INS_SEM);

    sem_close(pack_sem);
    sem_unlink(PACK_SEM);
}

void sig_exit(){
    exit(EXIT_SUCCESS);
}