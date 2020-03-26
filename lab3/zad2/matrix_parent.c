#include "matrix_parent.h"

int count_lines(char *path){
    FILE *file = fopen(path, "r");
    int res = 0;

    char *resp = NULL;
    size_t len = 0;
    while(getline(&resp, &len, file) != -1)
        res++;

    fclose(file);
    free(resp);
    return res;
}

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

void free_tasks(Task *tasks, int n){
    while(--n >= 0){
        free(tasks[n].first);
        free(tasks[n].second);
        free(tasks[n].result);
    }
    free(tasks);
}

int read_tasks(char *path, Task *tasks){
    int n = count_lines(path);
    tasks = malloc(n*sizeof(Task));

    FILE *list = fopen(path, "r");
    assert(list);
    char *line = NULL;
    size_t len = 0;
    int k = 0;

    while(getline(&line, &len, list) >= 0){
        char *matrix[3];
        for(int i = 0; i<3; i++)
            matrix[i] = malloc(len);

        int b = 0, e = 0;
        for(int i = 0; i<3; i++){
            while(e < len && line[e]!=' ')
                e++;
            strncpy(matrix[i], line+b, e-b+1);
            matrix[i][e-b+1] = '\0';
            e++;
            b = e;
        }

        tasks[k].first = matrix[0];
        tasks[k].second = matrix[1];
        tasks[k].result = matrix[2];
        k++;
    }
    fclose(list);
    return n;
}