#include <linux/limits.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h> 
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <time.h> 
#include <ftw.h>

void assert_args(bool x);
void print_usage();

void _show(char path[], int maxdepth){
    if(maxdepth == 0)
        return;

    printf("Path: %s\n",path);
    printf("Process pid: %d\n", (int)getpid());
    char cmd[6+PATH_MAX] = "ls -l ";
    strcat(cmd,path);
    system(cmd);
    printf("\n");

    DIR* dir = opendir(path);
    if(dir == NULL)
        return;

    struct dirent* dp;
    struct stat *buf = malloc(sizeof(struct stat));
    
    while((dp = readdir(dir)) != NULL){
        if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0)
            continue;

        char p[PATH_MAX];
        strcpy(p,path);
        strcat(p,"/");
        strcat(p,dp->d_name);

        if(lstat(p, buf) == -1)
            continue;

        if(!S_ISLNK(buf->st_mode) && S_ISDIR(buf->st_mode)){
            pid_t child = fork();
            if(child == 0){
                _show(p,maxdepth-1);
                break;
            }else
                waitpid(child,NULL, 0);
        }
    }
    free(buf);
    closedir(dir);
}

void show(char path[], int maxdepth){
    char absolute[PATH_MAX];
    assert(realpath(path,absolute) != NULL);

    _show(absolute,maxdepth);
}

int main(int argc, char *argv[]){
    static struct option long_options[] = {
        {"help",   no_argument, 0,  'h' },
        {"maxdepth",   required_argument, 0,  'd' },
        {0, 0, 0, 0}
    };      

    int long_index = 0;
    int opt = 0;
    int maxdepth = -2;

    while ((opt = getopt_long_only(argc, argv,"hnm:a:d:", long_options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
            case 'd': 
                assert_args(optarg > 0);
                maxdepth = atoi(optarg);                
                break;
            default: 
                assert_args(false);
        }
    }

    char path[PATH_MAX];
    path[0] = '.';
    path[1] = '\0';

    if(optind < argc)
        strcpy(path,argv[optind]);
    
    show(path, maxdepth);
    return 0;
}

void print_usage(){
    printf("Usage: ls [starting-point] \n");
    printf("Description: recursively run ls command to show files in a directory hierarchy.\n");
    printf("Options:\n");
    printf("    --help - show usage. \n");
    printf("    --maxdepth n - descend  at  most  n  (a non-negative integer) levels.\n");
    printf("\n");
}

void assert_args(bool trigger){
    if(trigger) 
        return;
    print_usage(); 
    exit(EXIT_FAILURE);
}