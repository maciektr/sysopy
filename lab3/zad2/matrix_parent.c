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

int read_tasks(char *path, Task **tasks, int n_workers){
    int n = count_lines(path);
    *tasks = malloc(n*sizeof(Task));

    FILE *list = fopen(path, "r");
    assert(list);
    char *line = NULL;
    size_t len = 0;
    int k = 0;

    while(getline(&line, &len, list) >= 0){
        (*tasks)[k].first = malloc(len);
        strcpy((*tasks)[k].first, strtok(line, " "));
        (*tasks)[k].second = malloc(len);
        strcpy((*tasks)[k].second, strtok(NULL, " "));
        (*tasks)[k].result = malloc(len);
        strcpy((*tasks)[k].result, strtok(NULL, " "));

        FILE *c = fopen((*tasks)[k].result, "w");
        fclose(c);

        int n_cols = count_cols((*tasks)[k].second);
        assert(n_cols > 0);
        (*tasks)[k].cols_per_worker = max((n_cols%n_workers == 0 ? n_cols / n_workers : n_cols/n_workers+1),1);
        k++;
    }
    free(line);
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

int merge_results(char *runtime_dir, Task *tasks, pid_t *workers){
    struct dirent **namelist;
    int n_file = scandir(runtime_dir, &namelist, 0, alphasort);
    assert(n_file >= 0);

    char **cmd_args = malloc((n_file+3) * sizeof(char *));
    cmd_args[0] = "paste";
    cmd_args[1] = "-d  ";
    for(int i = 2; i<=n_file+1; i++)
        cmd_args[i] = malloc(40);

    int cmd_id = 2; 
    int task_last = 0; 

    for(int n = 0; n < n_file; n++){
        if(strcmp(namelist[n]->d_name, ".") == 0 || strcmp(namelist[n]->d_name, "..") == 0){
            free(namelist[n]);
            continue;
        }
        char *name_cpy = malloc(strlen(namelist[n]->d_name)+1);
        strcpy(name_cpy, namelist[n]->d_name);
        char *stask_id = strtok(name_cpy, "_");
        assert(stask_id);
        int task_id = atoi(stask_id); 
        
        if(task_id == task_last){
            strcpy(cmd_args[cmd_id], runtime_dir);
            strcat(cmd_args[cmd_id], "/");
            strcat(cmd_args[cmd_id++], namelist[n]->d_name);
        }else{
            cmd_args[cmd_id++] = NULL;
            int child = fork();
            if(child == 0){
                int fd = open(tasks[task_id].result, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                dup2(fd,1);
                execvp("/usr/bin/paste", cmd_args);
                close(fd);
            }else
                workers[task_id] = child;

            for(int i = 2; i<cmd_id; i++){
                free(cmd_args[i]);
                cmd_args[task_id] = malloc(40);
            }
            cmd_id = 1;

            strcpy(cmd_args[cmd_id], runtime_dir);
            strcat(cmd_args[cmd_id], "/"); 
            strcat(cmd_args[cmd_id++], namelist[n]->d_name);
            task_last = task_id;
        }
        free(namelist[n]);
        free(name_cpy);
    }
    free(cmd_args[cmd_id]);
    cmd_args[cmd_id++] = NULL;
    
    int child  = fork();
    if(child == 0){
        int fd = open(tasks[task_last].result, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        dup2(fd,1);
        execvp("/usr/bin/paste", cmd_args);
        close(fd);
    }else
        workers[task_last] = child;

    free(namelist);
    for(int i = 2; i<= n_file+1; i++)
        free(cmd_args[i]);
    free(cmd_args);
    return task_last + 1;
}