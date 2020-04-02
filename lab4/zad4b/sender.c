#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h> 
#include <stdio.h>

enum mode_t {kill_m, sigqueue_m, sigrt_m};

pid_t pid;
mode_t mode = kill_m; 
int catched = 0; 
int n = 0;

void send(void){
    switch(mode){
        case kill_m:
            kill(pid, SIGUSR1);
            break;
        case sigqueue_m:{
            union sigval s = {.sival_int = SIGUSR1};
            sigqueue(pid, SIGUSR1, s);
            break;}
        case sigrt_m:
            kill(pid, SIGRTMIN);
            break;
    }
}

void send_end(void){
    switch(mode){
        case kill_m:
            kill(pid, SIGUSR2);
            break;
        case sigqueue_m:{
            union sigval s;
            s.sival_int = SIGUSR2;
            sigqueue(pid, SIGUSR2, s);
            break;}
        case sigrt_m:
            kill(pid, SIGRTMAX);
            break;
    }
}


void catch_usr1(int sig){
    catched++;
    if(--n)
        send();
    else 
        send_end();

}

void catch_usr2(int sig){
    printf("\nCatched %d\n", catched);
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]){
    assert(argc == 4);
    pid = (pid_t)atoi(argv[1]);
    n = atoi(argv[2]);

    char *mode_name = argv[3];
    if(strcmp(mode_name, "SIGQUEUE") == 0)
        mode = sigqueue_m;
    else if(strcmp(mode_name, "SIGRT") == 0)
        mode = sigrt_m;

    sigset_t all;
    sigfillset(&all);
    sigdelset(&all, SIGUSR1);
    sigdelset(&all, SIGUSR2);
    sigdelset(&all, SIGINT);
    sigdelset(&all, SIGRTMIN);
    sigdelset(&all, SIGRTMAX);
    assert(sigprocmask(SIG_BLOCK, &all, NULL) >= 0);

    signal(SIGUSR1,catch_usr1);
    signal(SIGUSR2,catch_usr2);
    printf("Sended: %d",n);
    send();
    while(1)
        pause();
}