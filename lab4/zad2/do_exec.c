#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h> 
#include <stdio.h>

void check_pending(int sigint){
    sigset_t pending;
    sigpending(&pending);
    if(sigismember(&pending,sigint) == 1)
        printf("Signal no. %d is pending\n", sigint);
    else if(sigismember(&pending,sigint) == 0)
        printf("Signal no. %d is not pending\n", sigint);
}

int main(int argc, char *argv[]){
    if(argc == 2 && strcmp(argv[1], "pending") == 0)
            check_pending(SIGUSR1);
    else 
        raise(SIGUSR1);
}