#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv){
    assert(argc >=2);
    char *path = argv[1];
    FILE *file = fopen(path, "r");
    assert(file);
    
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, file) != -1){

    }

    fclose(file);

}