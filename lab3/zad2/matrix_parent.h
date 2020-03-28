#ifndef _MATRIX_PARENT_H
#define _MATRIX_PARENT_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h> 
#include <unistd.h> 
#include <getopt.h>
#include <stdio.h>
#include <fcntl.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

struct Task{
    char *first;
    char *second; 
    char *result;
    int cols_per_worker;
};
typedef struct Task Task;
void free_tasks(Task *tasks, int n);
int read_tasks(char *path, Task **tasks, int n_workers);

int merge_results(char *runtime_dir, Task *tasks, pid_t *workers);
int count_cols(char *path);
int count_lines(char *path);

#endif