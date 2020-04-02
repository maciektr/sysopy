#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h> 
#include <stdio.h>

int catched=0;

void catcher_up(int sig){
    catched++;
}

void catch_end(int sig, siginfo_t * info, void *ucontext){
    printf("Catched %d\n", catched);
    while(catched--){
        kill(info->si_pid, SIGUSR1);
    }
    kill(info->si_pid, SIGUSR2);
    exit(EXIT_SUCCESS);
}

int main(){
    printf("My pid: %d\n", (int)getpid());

    sigset_t all;
    sigfillset(&all);
    sigdelset(&all, SIGUSR1);
    sigdelset(&all, SIGUSR2);
    sigdelset(&all, SIGINT);
    assert(sigprocmask(SIG_BLOCK, &all, NULL) >= 0);

    signal(SIGUSR1,catcher_up);
    signal(SIGRTMIN,catcher_up);

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &catch_end;
    sigaction(SIGUSR2,&act, NULL);
    sigaction(SIGRTMIN+1,&act, NULL);

    while(1) ;
}   