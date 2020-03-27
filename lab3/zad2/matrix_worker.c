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

    int n_rows = (row_max - row_min);
    int n_cols = (col_max - col_min);
    int **matrix = malloc(n_rows * sizeof(int *));
    for(int i = 0; i < n_rows; i++)
        matrix[i] = malloc(n_cols * sizeof(int));

    Matrix *encl = malloc(sizeof(Matrix));
    encl->matrix = matrix;
    encl->n_rows = n_rows;
    encl->n_cols = n_cols;

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
        char *pch = strtok(line, " \n");
        while(pch != NULL){
            if(act_col > col_max)
                break;
            if(act_col >= col_min)
                matrix[mat_row][mat_col++] = atoi(pch);
            act_col++;
            pch = strtok(NULL, " \n");
        }
        act_row++;
        mat_row++;
        mat_col = act_col = 0; 
    }
    
    free(line);
    fclose(file);
    return encl;
}

Matrix *load_whole(char *path){
    FILE *file = fopen(path, "r");
    assert(file != NULL);
    int n_cols = 0, n_rows = 0;

    char *line = NULL;
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
    for(int i = 0; i<ptr->n_rows; i++)
        free(ptr->matrix[i]);
    // while(--ptr->n_rows >= 0)
    //     free(ptr->matrix[ptr->n_rows]);
    free(ptr->matrix);
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

void print_matrix(Matrix *matrix){
    for(int k_r = 0; k_r < matrix->n_rows; k_r++){
            for(int k_c = 0; k_c < matrix->n_cols; k_c++)
                printf("%d ", matrix->matrix[k_r][k_c]);
            puts("");
        }
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

void fprint_matrix_pos(char *res_file_name, Matrix *matrix, int col_start, char *tmp_file_name){
    FILE *res_file = fopen(res_file_name, "r");
    assert(res_file);
    flock(fileno(res_file), LOCK_EX);

    char *line = NULL;
    size_t len = 0;

    FILE *tmp_res_f = fopen(tmp_file_name, "w");
    assert(tmp_res_f);

    int act_row = 0, act_col = 0; 
    while((getline(&line, &len, res_file)) != -1&& act_row < matrix->n_rows){
        char *tmp_res_line = malloc(len + (matrix->n_cols * 12 * 2));
        assert(tmp_res_line);

        char *pch = strtok(line, " \n");
        if(pch != NULL){
            strcpy(tmp_res_line, pch);
            strcat(tmp_res_line, " ");
            act_col++;
        }
        while((pch = strtok(NULL, " \n"))!= NULL && act_col < col_start){
            strcat(tmp_res_line, pch);
            strcat(tmp_res_line, " ");
            act_col++;
        }
        for(int i = 0; i < matrix->n_cols; i++){
            char snumb[11];
            sprintf(snumb, "%d ", matrix->matrix[act_row][i]);
            strcat(tmp_res_line, snumb);
        }
        while(pch != NULL){
            strcat(tmp_res_line, pch);
            strcat(tmp_res_line, " \n");
            act_col++;
            pch = strtok(NULL, " \n");
        }
        fprintf(tmp_res_f, "%s\n", tmp_res_line);

        free(tmp_res_line);
        act_row++;
        act_col = 0; 
    }      
    while(act_row < matrix->n_rows){
        while(act_col++ < col_start)
            fputc(' ',tmp_res_f);
        for(int i = 0; i < matrix->n_cols; i++)
            fprintf(tmp_res_f, "%d ", matrix->matrix[act_row][i]);
        act_col = 0; 
        act_row++;
        fputc('\n', tmp_res_f);
    }

    fclose(tmp_res_f);    
    fclose(res_file);
    tmp_res_f = fopen(tmp_file_name, "r");
    res_file = fopen(res_file_name, "w");

    while(getline(&line, &len, tmp_res_f) != -1)
        fputs(line,res_file);

    flock(fileno(res_file), LOCK_UN);
    fclose(res_file);
    fclose(tmp_res_f);
    free(line);
}

void fprint_matrix(char *filename, Matrix *matrix){
    FILE *file = fopen(filename, "w");
    assert(file);
    char *line = malloc(matrix->n_cols * 2 * 12 * sizeof(char));
    char *numb = malloc(12);

    for(int r = 0; r < matrix->n_rows; r++){
        sprintf(numb, "%d ", matrix->matrix[r][0]);
        strcpy(line, numb);

        for(int c = 1; c < matrix->n_cols; c++){
            sprintf(numb, "%d ", matrix->matrix[r][c]);
            strcat(line, numb);
        }
        strcat(line,"\n");
        fputs(line,file);
    }

    free(numb);
    free(line);
    fclose(file);
}

void merge_results(char *runtime_dir, Task *tasks){
    struct dirent **namelist;
    int n_file = scandir(runtime_dir, &namelist, 0, alphasort);
    assert(n_file >= 0);
    char *paste = "paste -d ' '";
    char *cmd = malloc(15 + n_file*30);
    strcpy(cmd, paste);
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
            strcat(cmd, " ");
            strcat(cmd, runtime_dir);
            strcat(cmd, "/");
            strcat(cmd, namelist[n]->d_name);
        }else{
            strcat(cmd, " > ");
            strcat(cmd, tasks[task_id].result);
            puts(cmd);
            system(cmd);

            strcpy(cmd, paste);
            strcat(cmd, " ");
            strcat(cmd, runtime_dir);
            strcat(cmd, "/");
            strcat(cmd, namelist[n]->d_name);
            task_last = task_id;
        }
        free(namelist[n]);
        free(name_cpy);
    }
    strcat(cmd, " > ");
    strcat(cmd, tasks[task_last].result);
    system(cmd);

    free(namelist);
    free(cmd);
}