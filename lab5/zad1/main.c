#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

int count_proc(char *line, int len){
    int res = 0;
    for(int i = 0; i < len; i++)
        if(line[i] == '|')
            res++;
    return res;
}

int count_args(char *cmd){
    char *pch = strtok(cmd, " |\n");
    int res = res==NULL ? 0 : 1;
    while(pch != NULL){
        res++;
        pch = strtok(NULL, " |\n");
    }
    return res;
}

char **get_cmd(char *cmd){
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

    char *pch = strtok(line, "|\n");
    while(pch != NULL){
        char **args = get_cmd(pch);
        

        pch = strtok(NULL, "|\n");
    }

    for(int i = 0; i<n_proc; i++)
        waitpid(proc[i], NULL, 0);
    free(proc);
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