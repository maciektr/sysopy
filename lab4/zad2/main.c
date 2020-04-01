#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h> 
#include <stdio.h>

void handler(int sigint){
    printf("Caught signal no. %d.\n",sigint);
}

void mask(int sigint){
    sigset_t new;
    sigemptyset(&new);
    sigaddset(&new, sigint);
    assert(sigprocmask(SIG_BLOCK, &new, NULL) >= 0);
}

void check_pending(int sigint){
    sigset_t pending;
    sigpending(&pending);
    if(sigismember(&pending,sigint) == 1)
        printf("Signal no. %d is pending\n", sigint);
    else if(sigismember(&pending,sigint) == 0)
        printf("Signal no. %d is not pending\n", sigint);
}

int main(int argc, char *argv[]){
    assert(argc == 2);

    if(strcmp(argv[1],"ignore") == 0){
        // ustawia ignorowanie
        signal(SIGUSR1, SIG_IGN);
    }else if(strcmp(argv[1],"handler") == 0){
        // instaluje handler obsługujący sygnał wypisujący komunikat o jego otrzymaniu
        signal(SIGUSR1, handler);
    }else if(strcmp(argv[1],"mask") == 0 || strcmp(argv[1],"pending") == 0)
        // maskuje ten sygnał
        mask(SIGUSR1);
        // oraz sprawdza (przy zamaskowaniu tego sygnału) czy wiszący/oczekujący sygnał jest widoczny w procesie

    // funkcji raise wysyła sygnał do samego siebie oraz wykonuje odpowiednie dla danej opcji działania
    raise(SIGUSR1);
    
    if(strcmp(argv[1],"pending") == 0)
        check_pending(SIGUSR1);

    // tworzy potomka funkcją fork
    int child = fork();
    if(child == 0){
    // ponownie przy pomocy funkcji raise potomek wysyła sygnał do samego siebie 
    // (z wyjątkiem opcji pending, gdzie testowane jest sprawdzenie, czy sygnał czekający w przodku jest widoczny w potomku).
        if(strcmp(argv[1], "pending") == 0)
            check_pending(SIGUSR1);
        else
            raise(SIGUSR1);        
    }else
        waitpid(child, NULL, 0);

    exit(EXIT_SUCCESS);
}       