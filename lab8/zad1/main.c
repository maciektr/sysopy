#include <linux/limits.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/time.h>
#include <stdbool.h> 
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <time.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>



int main(int argc, char *argv[]){
    assert(argc == 5);

    int n_threads = atoi(argv[1]);
    char *mode = argv[2];
    char *file_in = argv[3];
    char *file_out = argv[4];

    
}