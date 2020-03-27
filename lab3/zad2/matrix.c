#include <linux/limits.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <stdbool.h> 
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <time.h> 

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "matrix_parent.h"
#include "matrix_worker.h"

enum result_mode{common, separate};
typedef enum result_mode res_mod;

void assert_args(bool trigger);
void print_usage();

inline bool check_time(clock_t begin, int timeout){
    return (clock() - begin) / CLOCKS_PER_SEC > timeout;
}

int worker(int timeout, Task *tasks, int n_tasks, res_mod mode, int part){
    clock_t begin = clock();
    int res = 0;
    for(int task_id = 0; task_id < n_tasks; task_id++){
        if(check_time(begin, timeout))
            return res;

        int n_col = count_cols(tasks[task_id].second);

        if(part * tasks[task_id].cols_per_worker > n_col)
            continue;
            
        int col_start = part * tasks[task_id].cols_per_worker;
        int real_part = min(col_start + tasks[task_id].cols_per_worker, n_col) - col_start;
        if(check_time(begin, timeout))
            return res;

        Matrix *first = load_whole(tasks[task_id].first);
        // puts("first loaded");
        // print_matrix(first);
        
        if(check_time(begin, timeout))
            return res;

        Matrix *second = load_part(tasks[task_id].second, col_start, col_start+real_part, 0, -1);
        // puts("second loaded");
        // print_matrix(second);

        if(check_time(begin, timeout))
            return res;

        Matrix *result = multiply_matrices(first, second);
        res++;
        if(check_time(begin, timeout))
            return res;
        // puts("result");
        // print_matrix(result);

        if(mode == common){
            // printf("Proc %d otwiera res\n", (int)getpid());
            char *res_file_name = tasks[task_id].result;
            FILE *res_file = fopen(res_file_name, "r");
            assert(res_file);
            flock(fileno(res_file), LOCK_EX);

            char *line = NULL;
            size_t len = 0;

            char tmp_file_name[33];
            sprintf(tmp_file_name, ".runtime/.%dtmp", (int)getpid());
            FILE *tmp_res_f = fopen(tmp_file_name, "w");
            assert(tmp_res_f);

            int act_row = 0, act_col = 0; 
            while((getline(&line, &len, res_file)) != -1){// && act_row < result->n_rows){
                char *tmp_res_line = malloc(len + (result->n_cols * 12 * 2));
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
                for(int i = 0; i < result->n_cols; i++){
                    char snumb[11];
                    sprintf(snumb, "%d ", result->matrix[act_row][i]);
                    strcat(tmp_res_line, snumb);
                }
                while(pch != NULL){
                    strcat(tmp_res_line, pch);
                    strcat(tmp_res_line, " \n");
                    act_col++;
                    pch = strtok(NULL, " \n");
                }
                // printf("Will print: |%s\n", tmp_res_line);
                fprintf(tmp_res_f, "%s\n", tmp_res_line);

                free(tmp_res_line);
                act_row++;
                act_col = 0; 
            }      
            while(act_row < result->n_rows){
                while(act_col++ < col_start)
                    fputc(' ',tmp_res_f);
                for(int i = 0; i < result->n_cols; i++){
                    fprintf(tmp_res_f, "%d ", result->matrix[act_row][i]);
                }
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
        }else if(mode == separate){

        }

        free_matrix(first);
        free_matrix(second);
        free_matrix(result);
    }
    free_tasks(tasks, n_tasks);
    return res;
}

int main(int argc, char *argv[]){
    assert_args(argc == 5);

    int n_workers = atoi(argv[2]);
    assert_args(n_workers > 0);

    char *file_path = argv[1];
    Task *tasks = NULL;
    int n_tasks = read_tasks(file_path, &tasks, n_workers);

    int timeout = atoi(argv[3]);
    assert_args(timeout > 0);

    res_mod mode = atoi(argv[4]);
    assert_args(mode == common || mode == separate);

    mkdir(".runtime", 0777);

    pid_t *workers = malloc(n_workers * sizeof(pid_t));
    for(int i = 0; i<n_workers; i++){
        workers[i] = fork();
        if(workers[i] == 0){
            free(workers);
            return worker(timeout, tasks, n_tasks, mode, i);
        }
    }

    free_tasks(tasks, n_tasks);

    for(int i = 0; i<n_workers; i++){
        int status; 
        waitpid(workers[i], &status, 0);
        printf("Proces %d wykonał %d operacji mnożenia.\n", (int)getpid(), WEXITSTATUS(status));
    }

    free(workers);
    system("rm -rf .runtime");
    return 0;
}

void print_usage(){
    /*
    Argumenty: 
    - list.txt
    - n_workers
    - timeout time
    - sposob laczenia wyniku
    */
    puts("Usage: matrix [list] [workers] [timeout] [mode]"); 
    puts("Description: search for files in a directory hierarchy.");
    puts("Arguments:");
    puts("    - list - Path to file in which multiplication tasks are specified.");
    puts("    - workers - Defines the number of subprocesses created.");
    puts("    - timeout - Defines the maximal lifetime on subprocess in seconds.");
    puts("    - mode - Can be 1 or 2. Defines the way in which results of single\n multiplications are merged into the final matrix.");
    puts("Modes:");
    puts("    - 0 - common - Created subprocesses write results to a common file\n using FLOCK locks.");
    puts("    - 1 - separate - Created subprocesses write results to their own,\n separate files. The final result is created by another process.");
    puts("");
}

void assert_args(bool trigger){
    if(trigger) 
        return;
    print_usage(); 
    exit(EXIT_FAILURE);
}