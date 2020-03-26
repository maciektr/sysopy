#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <getopt.h>
#include <unistd.h> 
#include <time.h> 

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "matrix_parent.h"
#include "matrix_worker.h"

void assert_args(bool trigger);
void print_usage();

int worker(int timeout, Task *tasks, int n_tasks, res_mod mode){
    clock_t begin = clock();
    int res = 0;
    while(true){
        if((clock() - begin) / CLOCKS_PER_SEC > timeout)
            return res;
        
        
    }
    free_tasks(tasks, n_tasks);
    return res;
}

int main(int argc, char *argv[]){
    assert_args(argc == 5);
    char *file_path = argv[1];
    Task *tasks = NULL;
    int n_tasks = read_tasks(file_path, tasks);

    int n_workers = atoi(argv[2]);
    assert_args(n_workers > 0);

    int timeout = atoi(argv[3]);
    assert_args(timeout > 0);

    res_mod mode = atoi(argv[4]);
    assert_args(mode == common || mode == separate);

    pid_t *workers = malloc(n_workers * sizeof(pid_t));
    for(int i = 0; i<n_workers; i++){
        workers[i] = fork();
        if(workers[i] == 0){
            free(workers);
            return worker(timeout, tasks, n_tasks, mode);
        }
    }

    free_tasks(tasks, n_tasks);

    for(int i = 0; i<n_workers; i++){
        int status; 
        waitpid(workers[i], &status, 0);
        printf("Proces %d wykonał %d operacji mnożenia.\n", (int)getpid(), WEXITSTATUS(status));
    }

    free(workers);
    return 0;
}

/*
int n_collumns(char *path){
    FILE *file = fopen(path, "r");
    int res = 0;
    char c;
    while((c = fgetc(file)) != EOF){
        if(c == ' ')
            res++;
        else if(c == '\n')
            break;
    }
    fclose(file);
    return res+1;
}
*/

void print_usage(){
    /*
    Argumenty: 
    - list.txt
    - n_workers
    - timeout time
    - sposob laczenia wyniku
    */
    puts("Usage: matrix [list] [workers] [timeout] [mode]"); 
    puts("Description: search for files in a directory hierarchy.");
    puts("Arguments:");
    puts("    - list - Path to file in which multiplication tasks are specified.");
    puts("    - workers - Defines the number of subprocesses created.");
    puts("    - timeout - Defines the maximal lifetime on subprocess in seconds.");
    puts("    - mode - Can be 1 or 2. Defines the way in which results of single\n multiplications are merged into the final matrix.");
    puts("Modes:");
    puts("    - 1 - common - Created subprocesses write results to a common file\n using FLOCK locks.");
    puts("    - 2 - separate - Created subprocesses write results to their own,\n separate files. The final result is created by another process.");
    puts("");
}

void assert_args(bool trigger){
    if(trigger) 
        return;
    print_usage(); 
    exit(EXIT_FAILURE);
}