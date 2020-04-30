#include "common.h"

#include<stdio.h>

char *get_timestamp(){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    return asctime(timeinfo);
}

char *get_homedir(){
    char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    assert(homedir != NULL);
    return homedir;
}

int get_shm(){
    key_t key = ftok(get_homedir(), PROJECT_ID);
    assert(key != -1);
    int shm = shmget(key, SHM_SIZE, IPC_CREAT | IPC_EXCL | QMOD);
    if(shm != -1){
        shm_t *shmaddr = (shm_t *)shmat(shm, NULL, 0);
        shmaddr->insert_index = 0;
        shmaddr->pack_index = 0;
        shmaddr->remove_index = 0;
        assert(shmdt(shmaddr) == 0);
        return shm;
    }

    shm = shmget(key, 0, QMOD);
    assert(shm != -1);
    return shm;
}

int get_lock(){
    key_t key = ftok(get_homedir(), LOCK);
    assert(key != -1);
    
    int sem = semget(key, 1, IPC_CREAT | IPC_EXCL | QMOD);
    if(sem != -1){
        sem_qop(sem, 1);
        return sem; 
    }
    
    sem = semget(key, 1, QMOD);
    assert(sem != -1);
    return sem;
}

void set_sembuff(sembuf_t *buffer, int num, int op, int flag){
    buffer->sem_num = num;
    buffer->sem_op = op;
    buffer->sem_flg = flag;
}

void sem_qop(int lock_id, int op){
    sembuf_t buffer;
    set_sembuff(&buffer,0,op,0);
    semop(lock_id, &buffer, 1);
}

int n_to_pack(shm_t *shm){
    if(shm->insert_index >= shm->pack_index)
        return shm->insert_index - shm->pack_index;

    return shm->insert_index + (ORDERS_N - shm->pack_index);
}

int n_to_send(shm_t *shm){
    if(shm->pack_index >= shm->remove_index)
        return shm->pack_index - shm->remove_index;

    return shm->pack_index + (ORDERS_N - shm->remove_index);
}