#include <linux/limits.h>
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

    if(strcmp(argv[1],"ignore") == 0)
        signal(SIGUSR1, SIG_IGN);
    else if(strcmp(argv[1],"handler") == 0)
        signal(SIGUSR1, handler);
    else if(strcmp(argv[1],"mask") == 0 || strcmp(argv[1],"pending") == 0)
        mask(SIGUSR1);

    raise(SIGUSR1);
    
    if(strcmp(argv[1],"pending") == 0)
        check_pending(SIGUSR1);

    int child = fork();
    if(child == 0){
        if(strcmp(argv[1], "pending") == 0)
            check_pending(SIGUSR1);
        else
            raise(SIGUSR1);        
        return 0;
    }else
        waitpid(child, NULL, 0);

    char **cmd_args = malloc((2) * sizeof(char *));
    cmd_args[0] = "./do_exec.o";
    cmd_args[1] = argv[1];
    cmd_args[2] = NULL;

    char path[PATH_MAX];
    assert(realpath(".",path) != NULL);
    strcat(path,"/do_exec.o");
    execvp(path, cmd_args);
    free(cmd_args);
}       