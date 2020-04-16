#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv){
    assert(argc == 4);
    srand(time(NULL));
    char *fifo_path = argv[1];
    char *file_path = argv[2];
    int n_char = atoi(argv[3]);

    FILE *fifo = fopen(fifo_path, "a");
    FILE *file = fopen(file_path, "r");
    assert(fifo != NULL);
    assert(file != NULL);
    char *buffer = malloc((n_char + 2) * sizeof(char));
    int l;

    while((l = fread(buffer, 1, n_char, file)) > 0){
        buffer[l] = '\0';
        fprintf(fifo, "#%d#%s", (int)getpid(), buffer);
        fflush(fifo);
        sleep(rand()%2+1);
    }

    free(buffer);
    fclose(fifo);
    fclose(file);
}