#include "matrix_parent.h"

int count_cols(char *path){
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

int read_tasks(char *path, Task *tasks, int n_workers){
    int n = count_lines(path);
    tasks = malloc(n*sizeof(Task));

    FILE *list = fopen(path, "r");
    assert(list);
    char *line = NULL;
    size_t len = 0;
    int k = 0;

    while(getline(&line, &len, list) >= 0){
        tasks[k].first = malloc(len);
        strcpy(tasks[k].first, strtok(line, " "));
        tasks[k].second = malloc(len);
        strcpy(tasks[k].second, strtok(NULL, " "));
        tasks[k].result = malloc(len);
        strcpy(tasks[k].result, strtok(NULL, " "));

        int n_cols = count_cols(tasks[k].second);
        assert(n_cols > 0);
        n_cols += n_workers - n_cols%n_workers;
        tasks[k].cols_per_worker = max(n_cols / n_workers,1);
        k++;
    }
    fclose(list);
    return n;
}

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
