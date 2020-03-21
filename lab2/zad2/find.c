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
/*
mtime n
File's  data was last modified n*24 hours ago.  See the comments for -atime to understand how round‐
ing affects the interpretation of file modification times.
atime n
File was last accessed n*24 hours ago.  When find figures out how many 24-hour periods ago the  file
was  last  accessed,  any fractional part is ignored, so to match -atime +1, a file has to have been
accessed at least two days ago.
maxdepth levels
Descend  at  most  levels  (a non-negative integer) levels of directories below the starting-points.
-maxdepth 0
means only apply the tests and actions to the starting-points themselves.
*/

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

    int mtime;
    int atime;
    int maxdepth;
    bool nftw;
    
    while ((opt = getopt_long_only(argc, argv,"hm:a:d:", long_options, &long_index )) != -1) {
        // if (optarg)
            // printf ("%d with arg %s\n",optind, optarg);
        switch (opt) {
            case 'h':
                print_usage();
            case 'm': 
                    
                break;
            case 'a': 
             
                break;
            case 'd': 
                if(optarg < 0){
                    print_usage(); 
                    exit(EXIT_FAILURE);
                }
                
                break;
            default: print_usage(); 
                exit(EXIT_FAILURE);
        }
    }
    // while(optind < argc){
    //     printf("arg %s\n", argv[optind]);
    //     optind++;
    // }
    char *path = ".";
    char *name = NULL;
    if(optind+1 < argc){
        path = argv[optind];
        name = argv[optind+1];
    }else if(optind < argc){
        name = argv[optind];
    }else{
        print_usage(); 
        exit(EXIT_FAILURE);
    }
    


    return 0;
}