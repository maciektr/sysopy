#include "common.h"

char *get_timestamp(){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *res = asctime(timeinfo);
    for(char *p=res; *p; p++)
        if(*p == '\n')
            *p='\0';
    return res;
}

shm_t *get_shm(){
    int shm = shm_open(SHM_NAME, O_CREAT | O_EXCL, QMOD);
    if(shm != -1){  
        assert(ftruncate(shm, SHM_SIZE) == 0);
    }else{
        shm = shm_open(SHM_NAME, 0, QMOD);
        assert(shm != -1);
    }
    shm_t *shmaddr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, 0, shm, 0);
    assert(shmaddr != NULL);
    return shmaddr;
}

sem_t *get_lock(char *name, int val){
    sem_t *sem = sem_open(name, O_CREAT, QMOD, val);
    assert(sem != SEM_FAILED);
    return sem;
}

int n_to_pack(){
    int val;
    assert(sem_getvalue(get_lock(PACK_SEM, 0), &val) == 0);
    return val;
}

int n_to_send(){
    int val;
    assert(sem_getvalue(get_lock(SEND_SEM, 0), &val) == 0);
    return val;
}