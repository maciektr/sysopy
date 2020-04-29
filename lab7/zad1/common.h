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
#define ORDERS_N 1000
#define SHM_SIZE (ORDERS_N * sizeof(int))

char *get_timestamp();
char *get_homedir();
int get_shm();

#endif //_COMMON_H