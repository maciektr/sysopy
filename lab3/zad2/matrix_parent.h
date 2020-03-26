#ifndef _MATRIX_PARENT_H
#define _MATRIX_PARENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

int count_lines(char *path);

struct Task{
    char *first;
    char *second; 
    char *result;
};
typedef struct Task Task;
void free_tasks(Task *tasks, int n);
int read_tasks(char *path, Task *tasks);

enum result_mode{common, separate};
typedef enum result_mode res_mod;

#endif