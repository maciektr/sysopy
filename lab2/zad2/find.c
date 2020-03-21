#include <sys/types.h> 
#include <sys/stat.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h> 
#include <stdio.h>
#include <fcntl.h> 
#include <stdio.h>

void print_usage(){
    printf("Usage: find [options] [starting-point] [name] \n");
    printf("Description: search for files in a directory hierarchy.\n");
    printf("Options:\n");
    printf("    --help - show usage \n");
    printf("    --mtime n - file was last modified n*24 hours ago. Negative n means at least, positive utmost.\n");
    printf("    --atime n - file was last accessed n*24 hours ago. Negative n means at least, positive utmost.\n");
    printf("    --maxdepth n - descend  at  most  n  (a non-negative integer) levels\n");
}

void assert_args(bool x){
    if(x) 
        return;
    print_usage(); 
    exit(EXIT_FAILURE);
}

void find(char *path, char *name, int mtime, int atime, int maxdepth, bool nftw){
    
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
    int maxdepth = -1;
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

    char *path = ".";
    char *name = NULL;

    if(optind+1 < argc){
        path = argv[optind];
        name = argv[optind+1];
    }else if(optind < argc){
        name = argv[optind];
    }else{
        assert_args(false);
    }

    find(path, name, mtime, atime, maxdepth, nftw);

    return 0;
}