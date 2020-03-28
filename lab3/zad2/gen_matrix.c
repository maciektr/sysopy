#include <linux/limits.h>
#include <sys/types.h> 
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

void assert_args(bool trigger);
void print_usage();

void print_rand_matrix(char *filename, int n_rows, int n_cols, int minv, int maxv){
    FILE *file = fopen(filename, "w");
    assert(file);

    for(int k = 0; k < n_rows; k++){
        for(int i = 0; i < n_cols; i++){
            int rint = rand() % (maxv - minv + 1) + minv;
            fprintf(file, "%d ", rint);
        }
        fputc('\n', file);
    }

    fclose(file);
}

void generate(char *list_filename, char *folder, int n, int minv, int maxv, int minn, int maxn){
    pid_t *workers = malloc(n * sizeof(pid_t));
    if(folder != NULL){
        char rm_cmd[40] = "rm -rf ";
        strcat(rm_cmd, folder);
        system(rm_cmd);
        mkdir(folder, 0777);
    }   
    FILE *list = NULL;
    if(list_filename != NULL){
        list = fopen(list_filename, "w");
        fclose(list);
    }
    
    
    for(int i = 0; i<n; i++){
        if(list_filename != NULL)
            list = fopen(list_filename, "a");

        char *line = malloc(PATH_MAX  + 30);
        line[0] = '\0';
        char *a = malloc(PATH_MAX);
        char *b = malloc(PATH_MAX);
        if(folder != NULL){
            sprintf(a, "%s/%d_a.txt",folder, i);
            sprintf(b, "%s/%d_b.txt", folder, i);
            sprintf(line, "%s %s %s/%d_c.txt", a, b, folder, i);
        }else {
            sprintf(a, "%d_a.txt", i);
            sprintf(b, "%d_b.txt", i);
            sprintf(line, "%s %s %d_c.txt", a, b, i);
        }
        if(list != NULL){
            fprintf(list, "%s\n", line);
        }else{
            puts(line);
        }
        if(list != NULL)
            fclose(list);

        workers[i] = fork();
        if(workers[i] == 0){
            srand(time(NULL) ^ (getpid()<<16));
            free(line);
            free(workers);
            int side_a = rand() %(maxn - minn +1) + minn;
            int side_b = rand() %(maxn - minn +1) + minn;
            int side_c = rand() %(maxn - minn +1) + minn;
            print_rand_matrix(a, side_a, side_b, minv, maxv);
            print_rand_matrix(b, side_b, side_c, minv, maxv);
            free(a);
            free(b);
            exit(EXIT_SUCCESS);
        }
        free(a);
        free(b);
        free(line);
    }
    free(workers);
}

int main(int argc, char *argv[]){
    srand(time(NULL));
    static struct option long_options[] = {
        {"help",   no_argument, 0,  'h' },
        {"minv",   required_argument, 0,  'i' },
        {"maxv",   required_argument, 0,  'a' },
        {"minn",   required_argument, 0,  'o' },
        {"maxn",   required_argument, 0,  's' },
        {"list",   required_argument, 0,  'l' },
        {"numb",   required_argument, 0,  'n' },
        {"folder",   required_argument, 0,  'f' },
        {0, 0, 0, 0}
    };      

    int long_index = 0;
    int opt = 0;
    int minv = INT_MIN, maxv = INT_MAX;
    int minn = 1, maxn = 1;
    char *list_filename = NULL;
    char *folder_path = NULL;
    int n = 0;
    while ((opt = getopt_long_only(argc, argv,"hi:a:l:n:f:o:s:", long_options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                break;
            case 'i': 
                minv = atoi(optarg);
                break;
            case 'a': 
                maxv = atoi(optarg);
                break;
            case 'l':
                list_filename = optarg;
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'o':
                minn = atoi(optarg);
                break;
            case 's':
                maxn = atoi(optarg);
                break;
            case 'f':
                folder_path = optarg;
                break;
            default: 
                assert_args(false);
        }
    }
    assert(minv <= maxv);
    assert(minn <= maxn);
    generate(list_filename, folder_path, n, minv, maxv, minn, maxn);
    return 0;
}

void print_usage(){
    printf("Usage: gen [options]\n");
    printf("Description: Generates random matrices for testing purpose.\n");
    printf("Options:\n");
    printf("    --help - show usage. \n");
    printf("    --minv v - Defines minimal value in generated random matrix.\n");
    printf("    --maxv v - Defines maximal value in generated random matrix.\n");
    printf("    --minn n - Defines minimal side size of generated random matrix.\n");
    printf("    --maxn n - Defines maximal side size of generated random matrix.\n");
    printf("    --list l - Defines file to which the list of generated matrices will be printed. Unless used, the result should be printed to std out.\n");
    printf("    --numb n - Defines number of pairs of matrices to be generated.\n");
    printf("    --folder f - Place all matrices created in folder. Will be stated in path provided to list / stdout.\n");
    printf("\n");
}

void assert_args(bool trigger){
    if(trigger) 
        return;
    print_usage(); 
    exit(EXIT_FAILURE);
}