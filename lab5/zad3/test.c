#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main(){
    assert(mkfifo("fifo_file", 0777) == 0);

    for(int i = 0; i<5; i++){
        char *file = malloc(5);
        sprintf(file, "t%d.txt", i+1);
        if(fork()){
            execl("./manf.out", "./manf.out", "fifo_file", file, "10", NULL);
        }
        free(file);
    }
    if(fork())
        execl("./cons.out", "./cons.out", "fifo_file", "o.txt", "10", NULL);

    while(wait(NULL)>0);
}