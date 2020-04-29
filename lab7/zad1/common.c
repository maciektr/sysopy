#include "common.h"

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
    int shm = shmget(key, 0, 0);
    if(shm != -1)
        return shm;

    return shmget(key, SHM_SIZE,IPC_CREAT);
}
