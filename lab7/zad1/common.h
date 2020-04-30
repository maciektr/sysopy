#ifndef _COMMON_H
#define _COMMON_H

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h> 
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <pwd.h>

#define PROJECT_ID 244
#define LOCK 166
#define ORDERS_N 10
#define SHM_SIZE ((ORDERS_N + 3) * sizeof(int))
#define QMOD 0666

#define SEM_COUNT 4
typedef enum {
    ACC_SEM,
    INS_SEM,
    PACK_SEM,
    SEND_SEM 
} semrole_t;

typedef struct{
    int orders[ORDERS_N];
    int insert_index;
    int pack_index;
    int remove_index;
} shm_t;
typedef struct sembuf sembuf_t;

char *get_timestamp();
char *get_homedir();
int get_shm();
int get_lock();
void set_sembuff(sembuf_t *buffer, int num, int op, int flag);
void sem_2qop(int lock_id, int s1_id, int op1, int s2_id, int op2);
void sem_qop(int lock_id, int sem_id, int op);
int n_to_pack();
int n_to_send();

#endif //_COMMON_H