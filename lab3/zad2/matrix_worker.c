#include "matrix_worker.h"

Matrix *load_part(char *path, int col_min, int col_max, int row_min, int row_max){
    if(row_max == -1){
        row_min = 0; 
        row_max = count_rows(path);
    }

    assert(row_min <= row_max);
    assert(col_min <= col_max);

    if(row_min > row_max || col_min > col_max)
        return NULL;

    int n_rows = (row_max - row_min + 1);
    int n_cols = (col_max - col_min +1);
    int **matrix = malloc(n_rows * sizeof(int *));
    for(int i = 0; i < n_rows; i++)
        matrix[i] = malloc(n_cols * sizeof(int));

    FILE *file = fopen(path, "r");
    int act_row = 0, act_col = 0; 
    int mat_row = 0, mat_col = 0;

    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, file) != -1){      
        if(act_row < row_min){
            act_row++;
            continue;
        }else if(act_row >= row_max)
            break;

        char *pch = strtok(line, " ");
        while(pch != NULL){
            if(act_col < col_min){
                act_col++;
                continue;
            }else if(act_col >= col_max)
                break;
            
            matrix[mat_row][++mat_col] = atoi(pch);

            act_col++;
            pch = strtok(NULL, " ");
        }

        act_row++;
        mat_row++;
        mat_col = 0; 
    }
    
    free(line);
    fclose(file);
    Matrix *encl = malloc(sizeof(Matrix));
    encl->matrix = matrix;
    encl->n_rows = n_rows;
    encl->n_cols = n_cols;
    return encl;
}

Matrix *load_whole(char *path){
    FILE *file = fopen(path, "r");
    int n_cols = 0, n_rows = 0;

    char *line;
    size_t len;
    int glstat = getline(&line, &len, file);
    for(int i = 0; i<len; i++)
        if(line[i] == ' ')
            n_cols++;
    n_cols++;
    
    if(glstat != -1){
        n_rows=1;
        while(getline(&line, &len, file) != -1)
            n_rows++;
    }

    free(line);
    fclose(file);
    return load_part(path, 0, n_cols, 0, n_rows);
}

void free_matrix(Matrix *ptr){
    while(--ptr->n_rows)
        free(ptr->matrix[ptr->n_rows]);
    free(ptr);
}

int count_rows(char *path){
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

Matrix *alloc_matrix(int n_rows, int n_cols){
    Matrix *result = malloc(sizeof(Matrix));
    result->n_rows = n_rows;
    result->n_cols = n_cols;
    result->matrix = malloc(result->n_rows * sizeof(int *));
    for(int i = 0; i < result->n_rows; i++)
        result->matrix[i] = calloc(result->n_cols, sizeof(int));
    return result;
}

Matrix *multiply_matrices(Matrix *first, Matrix *second){
    if(first->n_cols != second->n_rows)
        return NULL;
    Matrix *result = alloc_matrix(first->n_rows, second->n_cols);
    
    for(int x = 0; x < result->n_rows; x++)
        for(int y = 0; y < result->n_cols; y++){
            result->matrix[x][y] = 0;
            for(int i = 0; i < second->n_rows; i++)
                result->matrix[x][y] += first->matrix[x][i] * second->matrix[i][y];
        }

    return result;
}