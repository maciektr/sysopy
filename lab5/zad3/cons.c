#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv){
    assert(argc == 4);
    char *fifo_path = argv[1];
    char *file_path = argv[2];
    int n_char = atoi(argv[3]);

    FILE *file = fopen(file_path, "w");
    
    fclose(file);
}