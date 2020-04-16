#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

int count_proc(char *line, int len){
    int res = 1;
    for(int i = 0; i < len; i++)
        if(line[i] == '|')
            res++;
    return res;
}

int count_args(char *in){
    char *cmd = malloc(strlen(in)+2);
    strcpy(cmd, in);
    char *pch = strtok(cmd, " |\n");
    int res = (pch==NULL ? 0 : 1);
    while(pch != NULL){
        res++;
        pch = strtok(NULL, " |\n");
    }
    free(cmd);
    return res;
}

char **get_cmd(char *in){
    char *cmd = malloc(strlen(in)+2);
    strcpy(cmd, in);
    char **res = malloc((count_args(cmd) + 1) * sizeof(char *));
    int i = 0;
    char *pch = strtok(cmd, " |\n");
    while(pch != NULL){
        res[i++] = pch;
        // printf("G %s\n", res[i-1]);
        pch = strtok(NULL, " |\n");
    }
    res[i] = NULL;
    // free(cmd);
    // printf("G %s\n", res[0]);
    // printf("G %s\n", res[1]);
    return res;
}

void process_line(char *line, int len){
    int n_proc = count_proc(line, len);
    printf("N_PROC: %d\n", n_proc);
    pid_t *proc = malloc(n_proc * sizeof(pid_t));

    int k = 0;
    int **fd_tab = malloc(2*sizeof(int *));
    fd_tab[0] = malloc(2 * sizeof(int));
    fd_tab[1] = malloc(2 * sizeof(int));
    char *pch = strtok(line, "|\n");

    int pch_off = 0;
    while(pch != NULL){
        printf("CMD %s\n", pch);
        pch_off += strlen(pch);
        char **args = get_cmd(pch);

        if(k < n_proc-1){
            puts("PIPE");
            assert(pipe(fd_tab[k%2]) != -1);
        }

        if((proc[k] = fork()) == 0){
            printf("S %s\n", args[0]);
            // In child process
            if(k > 0){
                printf("%s AS IN %d\n",args[0],k);
                // Use pipe as stdin
                dup2(fd_tab[(k+1)%2][0], STDIN_FILENO);
                close(fd_tab[(k+1)%2][1]);
            }
            if(k < n_proc-1){
                printf("%s AS OUT %d\n",args[0],k);
                // Use pipe as stdout
                dup2(fd_tab[k%2][1], STDOUT_FILENO);
                close(fd_tab[k%2][0]);
            }
            // printf("EXEC %s\n", args[0]);
            execvp(*args, args);
        }

        k++;
        free(args);
        pch = strtok(line+pch_off+1, "|\n");
    }

    for(int i = 0; i<=k; i++)
        waitpid(proc[i], NULL, 0);
    free(proc);
    free(fd_tab[0]);
    free(fd_tab[1]);
    free(fd_tab);
}

int main(int argc, char **argv){
    assert(argc >=2);
    char *path = argv[1];
    FILE *file = fopen(path, "r");
    assert(file);
    
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, file) != -1)
        process_line(line, len);

    free(line);
    fclose(file);
}