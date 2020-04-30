#ifndef _COMMON_H
#define _COMMON_H

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>

#define ORDERS_N 10
#define SHM_SIZE ((ORDERS_N + 3) * sizeof(int))
#define QMOD (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define SHM_NAME "/shop_shm_"
#define INS_SEM "/shop_sem_ins"
#define PACK_SEM "/shop_sem_pack"
#define SEND_SEM "/shop_sem_send"

typedef struct{
    int orders[ORDERS_N];
    int insert_index;
    int pack_index;
    int remove_index;
} shm_t;
typedef struct sembuf sembuf_t;

char *get_timestamp();
shm_t *get_shm();
sem_t *get_lock(char *name, int val);
int n_to_pack();
int n_to_send();

#endif //_COMMON_H