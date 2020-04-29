#ifndef _COMMON_H
#define _COMMON_H

#include <sys/shm.h> 
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <time.h>

#define PROJECT_ID 244
#define LOCK 164
#define ORDERS_N 1000
#define SHM_SIZE (ORDERS_N * sizeof(int))
#define QMOD 0666

char *get_timestamp();
char *get_homedir();
int get_lock();

#endif //_COMMON_H