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

void send(pid_t pid, int n, mode_t mode){
    for(int i = 0; i<n; i++){
        if(mode == kill_m)
            kill(pid, SIGUSR1);
        else if(mode == sigqueue_m){
            union sigval s = {.sival_int = SIGUSR1};
            sigqueue(pid, SIGUSR1, s);
        }else if(mode == sigrt_m)
            kill(pid, SIGRTMIN);
    }
    if(mode == kill_m)
        kill(pid, SIGUSR2);
    else if(mode == sigqueue_m){
        union sigval s;
        s.sival_int = SIGUSR2;
        sigqueue(pid, SIGUSR2, s);
    }else if(mode == sigrt_m)
            kill(pid, SIGRTMAX);
}

int catched = 0; 
int n = 0;

void catch_usr1(int sig){
    catched=catched+1;
}

void catch_usr2(int sig){
    printf("Sended: %d\nCatched %d\n", n, catched);
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]){
    assert(argc == 4);
    int catcher_pid = atoi(argv[1]);
    n = atoi(argv[2]);

    char *mode_name = argv[3];
    mode_t mode = kill_m; 
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

    send((pid_t)catcher_pid, n, mode);
    
    while(1)
        pause();
}