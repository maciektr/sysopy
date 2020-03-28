#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int running = 1;

void handle_sig(){
    if(running){
        running = 0; 
        puts("\nWaiting for CTRL+Z (continue) or CTRL+C (kill).");
        pause();
    }else
        running = 1;
}

void end(){
    exit(EXIT_SUCCESS);
}

int main(){
    signal(SIGINT,end);

    struct sigaction act;
    act.sa_handler = handle_sig;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    sigaction(SIGTSTP, &act, NULL);

    while(1){
        DIR* dir = opendir(".");
        
        struct dirent *dp;
        while((dp = readdir(dir)) != NULL){
            puts(dp->d_name);
        }

        closedir(dir);
    }
}