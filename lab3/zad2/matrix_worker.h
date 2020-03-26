#ifndef _MATRIX_WORKER_H
#define _MATRIX_WORKER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int count_rows(char *path);

struct Matrix{
    int **matrix;
    int n_rows;
    int n_cols;
};
typedef struct Matrix Matrix;
Matrix *load_part(char *path, int col_min, int col_max, int row_min, int row_max);
Matrix *load_whole(char *path);
void free_matrix(Matrix *ptr);

#endif