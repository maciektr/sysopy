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

    FILE *fifo = fopen(fifo_path, "r");
    FILE *file = fopen(file_path, "w");
    char *buffer = malloc((n_char + 2) * sizeof(char));
    int l;

    while((l = fread(buffer, 1, n_char, fifo)) > 0)
        fwrite(buffer, 1, l, file);

    fclose(fifo);
    fclose(file);
}