#ifndef _MATRIX_WORKER_H
#define _MATRIX_WORKER_H

#include "matrix_parent.h"
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h> 
#include <dirent.h> 
#include <stdio.h>

int count_rows(char *path);

struct Matrix{
    int **matrix;
    int n_rows;
    int n_cols;
};
typedef struct Matrix Matrix;

Matrix *load_part(char *path, int col_min, int col_max, int row_min, int row_max);
void fprint_matrix_pos(char *res_file_name, Matrix *result, int col_start, char *tmp_file_name);
Matrix *multiply_matrices(Matrix *first, Matrix *second);
void fprint_matrix(char *filename, Matrix *matrix);
Matrix *alloc_matrix(int n_rows, int n_cols);
void merge_results(char *runtime_dir, Task *tasks);
void print_matrix(Matrix *matrix);
Matrix *load_whole(char *path);
void free_matrix(Matrix *ptr);

#endif