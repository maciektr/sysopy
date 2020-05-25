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
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <time.h> 

void atexit_handle();
void stop_sig();

int main(int argc, char *argv[]){
    assert(argc == 4);

    char *name = argv[1];
    char *connection_mode = argv[2];
    char *server_ip = argv[3];
    
    atexit(atexit_handle);
    signal(SIGINT, stop_sig);

}

void atexit_handle() {

}

void stop_sig(){
    exit(EXIT_SUCCESS);
}