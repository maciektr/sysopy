#include <sys/types.h> 
#include <sys/stat.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h> 
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <time.h> 


void assert_args(bool x);
void print_usage();
char *file_type(mode_t mode);


void print_stat(char *path, char *name, struct stat *buf){
    printf("Found: %s\n", name);
    printf("Path: %s\n", path);
    printf("Links number: %d\n", (int)buf->st_nlink);
    printf("File type: %s\n", file_type(buf->st_mode));
    printf("Size in bytes: %d\n", (int)buf->st_size);
    printf("Last access: %s",asctime(localtime(&buf->st_atime)));
    printf("Last modification: %s",asctime(localtime(&buf->st_mtime)));
    printf("\n");
}

void _search_stat(char path[], char *name, int mtime, int atime, int maxdepth){
    if(maxdepth == -1)
        return;
    DIR* dir = opendir(path);
    struct dirent* dp;
    struct stat *buf = malloc(sizeof(struct stat));

    while((dp = readdir(dir)) != NULL){
        if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0)
            continue;

        char p[PATH_MAX];
        strcpy(p,path);
        strcat(p,"/");
        strcat(p,dp->d_name);
            
        assert(stat(p, buf) != -1);

        if(strstr(dp->d_name, name))
            print_stat(p,dp->d_name,buf);

        if(S_ISDIR(buf->st_mode))
            _search_stat(p,name,mtime,atime,maxdepth-1);
    }

    free(buf);
    closedir(dir);
}

void find(char path[], char *name, int mtime, int atime, int maxdepth, bool nftw){
    char absolute[PATH_MAX];
    assert(realpath(path,absolute) != NULL);

    if(nftw){


    }else
        _search_stat(absolute,name,mtime,atime,maxdepth);
}

int main(int argc, char *argv[]){
    static struct option long_options[] = {
        {"help",   no_argument, 0,  'h' },
        {"mtime",   required_argument, 0,  'm' },
        {"atime",   required_argument, 0,  'a' },
        {"maxdepth",   required_argument, 0,  'd' },
        {"nftw", no_argument, 0, 'n'},
        // {"path",   required_argumen  , 0,  'p' },
        {0, 0, 0, 0}
    };      

    int long_index = 0;
    int opt = 0;

    int mtime = 0;
    int atime = 0;
    int maxdepth = -2;
    bool nftw = false;

    while ((opt = getopt_long_only(argc, argv,"hnm:a:d:", long_options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
            case 'm': 
                assert_args(optarg != NULL);
                mtime = atoi(optarg);
                break;
            case 'a': 
                assert_args(optarg != NULL);
                atime = atoi(optarg);
                break;
            case 'd': 
                assert_args(optarg > 0);
                maxdepth = atoi(optarg);                
                break;
            case 'n': 
                nftw = true;
                break;
            default: 
                assert_args(false);
        }
    }

    char path[PATH_MAX];
    path[0] = '.';
    path[1] = '\0';
    char *name = NULL;

    if(optind+1 < argc){
        strcpy(path,argv[optind]);
        name = argv[optind+1];
    }else if(optind < argc){
        name = argv[optind];
    }else{
        assert_args(false);
    }
    
    // if(path[0] != '/'){
    // }

    find(path, name, mtime, atime, maxdepth, nftw);

    return 0;
}

void print_usage(){
    printf("Usage: find [options] [starting-point] [name] \n");
    printf("Description: search for files in a directory hierarchy.\n");
    printf("Options:\n");
    printf("    --help - show usage. \n");
    printf("    --mtime n - file was last modified n*24 hours ago. Negative n means at least, positive utmost.\n");
    printf("    --atime n - file was last accessed n*24 hours ago. Negative n means at least, positive utmost.\n");
    printf("    --maxdepth n - descend  at  most  n  (a non-negative integer) levels.\n");
    printf("    --nftw - enforce nftw style implementation.");
}

void assert_args(bool x){
    if(x) 
        return;
    print_usage(); 
    exit(EXIT_FAILURE);
}

char *file_type(mode_t mode){
    if(S_ISFIFO(mode))
        return "fifo";
    if(S_ISDIR(mode))
        return "dir";
    if(S_ISLNK(mode))
        return "slink";
    if(S_ISBLK(mode))
        return "block dev";
    if(S_ISSOCK(mode))
        return "sock";
    if(S_ISCHR(mode))
        return "char dev";
    return "file";
}