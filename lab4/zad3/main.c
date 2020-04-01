#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h> 
#include <stdio.h>
#include <time.h>

void handler_SIGFPE(int sig, siginfo_t * info, void *ucontext){
    printf("Signal code: %d\n", info->si_code);
    exit(EXIT_SUCCESS);

}

void handler_SIGABRT(int sig, siginfo_t * info, void *ucontext){
    printf("User time consumed: %d\n", (int)info->si_utime);
    exit(EXIT_SUCCESS);
}

void handler_SIGSEGV(int sig, siginfo_t * info, void *ucontext){
    printf("User id: %d\n", info->si_uid);
    exit(EXIT_SUCCESS);
}

int main(){
    srand(time(NULL));
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &handler_SIGFPE;
    sigaction(SIGFPE,&act, NULL);

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &handler_SIGABRT;
    sigaction(SIGABRT,&act, NULL);

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &handler_SIGSEGV;
    sigaction(SIGSEGV,&act, NULL);
    
    int a = 20;
    if(fork() == 0)
        abort();

    if(fork() == 0){
        int b = 0;
        int fail = a/b;
    }
    if(fork() == 0){
        int *ptr = &a;
        ptr -= rand()%200+20000000;
        *ptr = 20050402;
    }

    wait(NULL);
}