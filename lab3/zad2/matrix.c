#include <linux/limits.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/time.h>
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

#define RUNTIME_DIR ".runtime"

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
        int real_part = min(col_start + tasks[task_id].cols_per_worker, n_col) - col_start -1;
        if(real_part <= 0)
            continue;
        if(check_time(begin, timeout))
            return res;

        Matrix *first = load_whole(tasks[task_id].first);      
        if(check_time(begin, timeout))
            return res;


        Matrix *second = load_part(tasks[task_id].second, col_start, col_start+real_part-1, 0, -1);
        if(check_time(begin, timeout))
            return res;

        Matrix *result = multiply_matrices(first, second);
        res++;
        if(check_time(begin, timeout))
            return res;
        
        char *res_file = malloc(33);
        sprintf(res_file, "%s/.%d_%d_%dtmp", RUNTIME_DIR, task_id, part, (int)getpid());

        if(mode == common)
            fprint_matrix_pos(tasks[task_id].result, result, col_start, res_file);
        else if(mode == separate)
            fprint_matrix(res_file, result);
        
        free(res_file);
        free_matrix(first);
        free_matrix(second);
        free_matrix(result);
    }
    free_tasks(tasks, n_tasks);
    return res;
}

int main(int argc, char *argv[]){
    static struct option long_options[] = {
        {"help",   no_argument, 0,  'h' },
        {"meml",   required_argument, 0,  'm' },
        {"cpul",   required_argument, 0,  'c' },
        {"stats", no_argument, 0, 's'},
        {0, 0, 0, 0}
    };      

    int long_index = 0;
    int opt = 0;
    int meml = -1;
    int cpul = -1;
    bool stats = false;
    while ((opt = getopt_long_only(argc, argv,"hm:c:s", long_options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                break;
            case 'm':
                meml = 1000000 * atoi(optarg);
                assert_args(meml >= 0);
                break;
            case 'c':
                cpul = atoi(optarg);
                assert_args(cpul >= 0);
                break;
            case 's':
                stats = true;
                break;
            default:
                assert_args(false);
        }
    }
    assert_args(optind + 3 < argc);

    int n_workers = atoi(argv[optind + 1]);
    assert_args(n_workers > 0);

    char *file_path = argv[optind];

    Task *tasks = NULL;
    int n_tasks = read_tasks(file_path, &tasks, n_workers);

    int timeout = atoi(argv[optind + 2]);
    assert_args(timeout > 0);

    res_mod mode = atoi(argv[optind + 3]);
    assert_args(mode == common || mode == separate);

    char rm_cmd[40] = "rm -rf ";
    strcat(rm_cmd, RUNTIME_DIR);
    system(rm_cmd);
    mkdir(RUNTIME_DIR, 0777);

    pid_t *workers = malloc(n_workers * sizeof(pid_t));
    for(int i = 0; i<n_workers; i++){
        workers[i] = fork();
        if(workers[i] == 0){
            free(workers);

            struct rlimit *limit = malloc(sizeof(struct rlimit));
            if(cpul != -1){
                getrlimit(RLIMIT_CPU, limit);
                limit->rlim_cur = min((int)limit->rlim_cur, cpul);
                limit->rlim_max = cpul;
                setrlimit(RLIMIT_CPU, limit);
            }
            if(meml != -1){
                getrlimit(RLIMIT_AS, limit);
                limit->rlim_cur = min((int)limit->rlim_cur, meml);
                limit->rlim_max = meml;
                setrlimit(RLIMIT_AS, limit);
            }
            free(limit);
            return worker(timeout, tasks, n_tasks, mode, i);
        }
    }

    for(int i = 0; i<n_workers; i++){
        int status; 
        waitpid(workers[i], &status, 0);
        printf("Proces %d wykonał %d operacji mnożenia.\n", (int)getpid(), WEXITSTATUS(status));
    }
    
    if(mode == separate){
        free(workers);
        workers = malloc(n_tasks * sizeof(pid_t));
        int last = merge_results(RUNTIME_DIR, tasks, workers);
        for(int i = 0; i < last; i++)
            waitpid(workers[i], NULL, 0);
    }

    if(stats){
        struct rusage *us = malloc(sizeof(struct rusage));
        assert(getrusage(RUSAGE_CHILDREN, us) == 0);
        puts("Resources used:");
        printf("User CPU time: %ld s %ld us\n", (long int)us->ru_utime.tv_sec, (long int)us->ru_utime.tv_usec);
        printf("System CPU time: %ld s %ld us\n", (long int)us->ru_stime.tv_sec, (long int)us->ru_utime.tv_usec);
        printf("Memory: %d KB\n", (int)us->ru_maxrss);
        free(us);
    }

    free_tasks(tasks, n_tasks);
    free(workers);
    system(rm_cmd);
    return 0;
}

void print_usage(){
    puts("Usage: matrix [options] [list] [workers] [timeout] [mode]"); 
    puts("Description: search for files in a directory hierarchy.");
    puts("Options:");
    puts("    --meml n - Defines memory limit (in megabytes) for every subprocess.");
    puts("    --cpul n - Defines cpu time limit (in seconds) for every subprocess.");
    puts("    --stats - Shows statistics of resources used after program termination.");
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