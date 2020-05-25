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

#define SHORT_WAIT 5
#define LONG_WAIT 10

int n_clients;
int n_chairs;
int next_client;
int next_chair;
int used_chairs;
int barber_sleeping;

pthread_t *chairs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void wait_for_threads(int n_threads, pthread_t *threads);
void random_sleep(int max);

void *barber_handler(void *arg){
    pthread_t shaved = 0;
    int clients_left = n_clients;
    while(1){
        pthread_mutex_lock(&mutex);
        if(used_chairs == 0){
            printf("Golibroda: idzie spac.\n");
            barber_sleeping = 1;
            pthread_cond_wait(&cond, &mutex);
            barber_sleeping = 0;
        }
        shaved = chairs[next_client++];
        next_client %= n_chairs;
        used_chairs--;
        printf("Golibroda: goli klienta %ld.\nGolibroda: w poczekalni %d klientow.\n", shaved, used_chairs);
        pthread_mutex_unlock(&mutex);
        random_sleep(LONG_WAIT);
        pthread_mutex_lock(&mutex);
        pthread_cancel(shaved);
        clients_left--;
        if(clients_left == 0)
            break;
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit((void *)0);
}

void *client_handler(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        if(used_chairs < n_chairs){
            chairs[next_chair++] = pthread_self();
            next_chair %= n_chairs;
            used_chairs++;
            printf("Klient %ld: siada w poczekalni.\n", pthread_self());
        }else{
            printf("Klient %ld: wszystkie krzesla zajete.\n",pthread_self());
            pthread_mutex_unlock(&mutex);
            random_sleep(SHORT_WAIT);
            continue;
        }
        if(barber_sleeping){
            printf("Klient %ld: budzi golibrode.\n", pthread_self());
            pthread_cond_broadcast(&cond);
            break;
        }
        pthread_mutex_unlock(&mutex);
        break;
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit((void *)0);
}

int main(int argc, char *argv[]){
    assert(argc == 3);
    srand(time(NULL));
    assert(pthread_mutex_init(&mutex, NULL) == 0);

    n_chairs = atoi(argv[1]);
    n_clients = atoi(argv[2]);

    chairs = (pthread_t *)malloc((n_chairs+1) * sizeof(pthread_t));
    next_client = next_chair = used_chairs = 0;
    barber_sleeping = 0;

    pthread_t *threads = (pthread_t *)malloc((n_clients + 1) * sizeof(pthread_t));
    pthread_create(&threads[0], NULL, barber_handler, NULL);
    for(int i = 1; i <= n_clients; i++){
        random_sleep(SHORT_WAIT);
        pthread_create(&threads[i], NULL, client_handler, NULL);
    }
    wait_for_threads(n_clients + 1, threads);
    free(threads);
    free(chairs);
}

void wait_for_threads(int n_threads, pthread_t *threads){
    while(--n_threads > 0)
        pthread_join(threads[n_threads],NULL);
}

void random_sleep(int max){
    sleep(rand() % max + 1);
}