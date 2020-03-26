#ifndef _MATRIX_PARENT_H
#define _MATRIX_PARENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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