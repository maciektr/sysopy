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
    int first = 0;
    int shm = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, QMOD);
    if(shm != -1){  
        assert(ftruncate(shm, SHM_SIZE) != -1);
        first = 1;
    }else{
        shm = shm_open(SHM_NAME, O_RDWR, QMOD);
        assert(shm != -1);
    }
    shm_t *shmaddr = (shm_t *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    assert(shmaddr != NULL);
    if(first){
        shmaddr->insert_index = 0;
        shmaddr->pack_index = 0;
        shmaddr->remove_index = 0;
    }
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