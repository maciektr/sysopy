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
        pch = strtok(NULL, " |\n");
    }
    res[i] = NULL;
    return res;
}

void process_line(char *line, int len){
    int n_proc = count_proc(line, len);
    pid_t *proc = malloc(n_proc * sizeof(pid_t));

    int k = 0;
    int **fd_tab = malloc(2*sizeof(int *));
    // int fda[2];
    // int fdb[2];
    // fd_tab[0] = fda;
    // fd_tab[1] = fdb;
    fd_tab[0] = malloc(2 * sizeof(int));
    fd_tab[1] = malloc(2 * sizeof(int));
    char *pch = strtok(line, "|\n");
    while(pch != NULL){
        printf("CMD %s\n", pch);
        char **args = get_cmd(pch);
        if(k != n_proc-1){
            pipe(fd_tab[k%2]);
        }
        if((proc[k] = fork()) == 0){
            if(k != 0){
                // use pipe as input
                // close(fd_tab[(k+1)%2][0]);
                dup2(fd_tab[(k+1)%2][1], STDIN_FILENO);
            }
            if(k != n_proc-1){
                // close(fd_tab[k%2][1]);
                dup2(fd_tab[k%2][0], STDOUT_FILENO);
            }
            // exec
            free(proc);
            free(fd_tab[0]);
            free(fd_tab[1]);
            free(fd_tab);
            execvp(*args, args+1);
        // }else{
        }
        free(args);

        // int fd[2];
        // pipe(fd);
        // pid_t pid = fork();
        // if (pid == 0) { // dziecko
        //     close(fd[1]); 
        //     dup2(fd[0],STDIN_FILENO);
        //     execlp("grep", "grep","Ala", NULL);
        // } else { // rodzic
        //     close(fd[0]);
        //     // write(fd[1], ...) - przesłanie danych do grep-a
        // }

        // pipe(fd);
        // if (fork() == 0) { // dziecko 1 - pisarz
        //     close(fd[0]);
        //     // ...
        // } else if (fork() == 0) { // dziecko 2 - czytelnik
        //     close(fd[1]);
        //     // ...
        // }
        k++;
        pch = strtok(NULL, "|\n");
    }

    for(int i = 0; i<n_proc; i++)
        waitpid(proc[i], NULL, 0);
    free(proc);
    free(fd_tab[0]);
    free(fd_tab[1]);
    free(fd_tab);
}

int main(int argc, char **argv){
    // char *cmd[] = {"-al",NULL};
    // execvp("ls",cmd);

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