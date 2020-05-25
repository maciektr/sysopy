#include <linux/limits.h>
#include <sys/resource.h>
#include <stdatomic.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/time.h>
#include <stdbool.h> 
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <time.h> 

#define NEXT_CLIENT_WAIT 3
#define JOB_WAIT 9

int n_clients;
int n_chairs;

pthread_t *chairs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void barber_handler(){
    while(1){

    }
    pthread_exit((void *)0);
}

void client_handler(){
    while(1){
        
    }
    pthread_exit((void *)0);
}

void wait_for_threads(int n_threads, pthread_t *threads);
void random_sleep(int max);

int main(int argc, char *argv[]){
    assert(argc == 3);
    srand(time(NULL));
    assert(pthread_mutex_init(&mutex, NULL) == 0);

    n_chairs = atoi(argv[1]);
    n_clients = atoi(argv[2]);

    chairs = (pthread_t *)malloc(n_chairs * sizeof(pthread_t));
    pthread_t *threads = (pthread_t *)malloc((n_clients + 1) * sizeof(pthread_t));
    pthread_create(&threads[0], NULL, barber_handler, NULL);
    for(int i = 1; i <= n_clients; i++){
        random_sleep(NEXT_CLIENT_WAIT);
        pthread_create(&threads[i], NULL, client_handler, NULL);
    }

    wait_for_threads(n_clients + 1, threads);
}

void wait_for_threads(int n_threads, pthread_t *threads){
    while(--n_threads >= 0)
        assert(pthread_join(threads[n_threads],NULL) == 0);
}

void random_sleep(int max){
    sleep(rand() % max + 1);
}