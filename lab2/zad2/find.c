#include <linux/limits.h>
#include <sys/types.h> 
#include <sys/stat.h>
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

struct time_arg{
    int mtime;
    int atime;

    // Sign variables define type of limit:
    // 0 - no limit
    // 1 - exact (no sign in program argument)
    // 2 - younger (- in argument)
    // 3 - older (+ in argument)
    char msign;
    char asign;
};

void print_stat(const char *path, const char *name, const struct stat *buf);
char *file_type(mode_t mode);
void assert_args(bool x);
void print_usage();


bool check_time(struct time_arg targ, const struct stat *buf){
    time_t now = time(NULL);
    int adiff = (int)difftime(now,buf->st_atime) / 3600 / 24;
    int mdiff = (int)difftime(now,buf->st_mtime) / 3600 / 24;
    
    if(targ.asign == 1 && adiff != targ.atime)
        return false;   
    if(targ.asign == 2 && adiff >= targ.atime)
        return false; 
    if(targ.asign == 3 && adiff <= targ.atime)
        return false; 

    if(targ.msign == 1 && mdiff != targ.mtime)
        return false; 
    if(targ.msign == 2 && mdiff >= targ.mtime)
        return false; 
    if(targ.msign == 3 && mdiff <= targ.mtime)
        return false; 
    
    return true;
}

int _search_dir(char path[], char *name, struct time_arg targ, int maxdepth){
    if(maxdepth == 0)
        return 0;

    DIR* dir = opendir(path);
    if(dir == NULL)
        return 0;

    struct dirent* dp;
    struct stat *buf = malloc(sizeof(struct stat));
    
    int res = 0;
    while((dp = readdir(dir)) != NULL){
        if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0)
            continue;

        char p[PATH_MAX];
        strcpy(p,path);
        strcat(p,"/");
        strcat(p,dp->d_name);

        if(lstat(p, buf) == -1)
            continue;

        if(strstr(dp->d_name, name) && check_time(targ, buf)){
            print_stat(p,dp->d_name,buf);
            res++;
        }

        if(!S_ISLNK(buf->st_mode) && S_ISDIR(buf->st_mode))
            res += _search_dir(p,name,targ,maxdepth-1);
    }
    free(buf);
    closedir(dir);
    return res;
}


int _search_nftw(char path[], char *name, struct time_arg targ, int maxdepth){
    int res = 0;
    
    int _process_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
        if(maxdepth >= 0 && ftwbuf->level > maxdepth && typeflag == FTW_D)
            return FTW_SKIP_SUBTREE;
        
        if((maxdepth >= 0 && ftwbuf->level > maxdepth) 
            || typeflag == FTW_NS
            || strcmp(path, fpath) == 0
          )
            return 0;
    
        const char *file = fpath + ftwbuf->base;

        if(strstr(file, name) && check_time(targ, sb)){
            print_stat(fpath,file,sb); 
            res++;
        }
        
        return 0;
    };

    assert(nftw(path, _process_file, 30, FTW_PHYS | FTW_ACTIONRETVAL) != -1);
    
    return res;
}

void find(char path[], char *name, struct time_arg targ, int maxdepth, bool nftw){
    char absolute[PATH_MAX];
    assert(realpath(path,absolute) != NULL);
    int res = 0;

    if(nftw)
        res = _search_nftw(absolute,name,targ,maxdepth);
    else
        res = _search_dir(absolute,name,targ,maxdepth);

    printf("Found total of %d files.\n", res);
}

int main(int argc, char *argv[]){
    static struct option long_options[] = {
        {"help",   no_argument, 0,  'h' },
        {"mtime",   required_argument, 0,  'm' },
        {"atime",   required_argument, 0,  'a' },
        {"maxdepth",   required_argument, 0,  'd' },
        {"nftw", no_argument, 0, 'n'},
        {0, 0, 0, 0}
    };      

    int long_index = 0;
    int opt = 0;

    struct time_arg targ;
    targ.asign = 0;
    targ.msign = 0;

    int maxdepth = -2;
    bool nftw = false;

    while ((opt = getopt_long_only(argc, argv,"hnm:a:d:", long_options, &long_index )) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
            case 'm': 
                assert_args(optarg != NULL);
                if(optarg[0] == '+')
                    targ.msign = 3;
                else if(optarg[0] == '-')
                    targ.msign = 2;
                else
                    targ.msign = 1;                
                targ.mtime = abs(atoi(optarg));
                break;
            case 'a': 
                assert_args(optarg != NULL);
                if(optarg[0] == '+')
                    targ.asign = 3;
                else if(optarg[0] == '-')
                    targ.asign = 2;
                else
                    targ.asign = 1;
                targ.atime = abs(atoi(optarg));
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
    char *name = "\0";

    if(optind + 2 < argc)
        assert_args(false);

    if(optind + 1 < argc){
        strcpy(path,argv[optind]);
        name = argv[optind+1];
    }else if(optind < argc)
        strcpy(path,argv[optind]);
    
    find(path, name, targ, maxdepth, nftw);
    return 0;
}

void print_usage(){
    printf("Usage: find [options] [starting-point] [name] \n");
    printf("Description: search for files in a directory hierarchy.\n");
    printf("Options:\n");
    printf("    --help - show usage. \n");
    printf("    --mtime n - file was last modified n*24 hours ago. Negative n means younger, positive older.\n");
    printf("    --atime n - file was last accessed n*24 hours ago. Negative n means younger, positive older.\n");
    printf("    --maxdepth n - descend  at  most  n  (a non-negative integer) levels.\n");
    printf("    --nftw - enforce nftw style implementation.\n");
    printf("\n");
}

void assert_args(bool trigger){
    if(trigger) 
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

void print_stat(const char *path, const char *name, const struct stat *buf){
    printf("Found: %s\n", name);
    printf("Path: %s\n", path);
    printf("Links number: %d\n", (int)buf->st_nlink);
    printf("File type: %s\n", file_type(buf->st_mode));
    printf("Size in bytes: %d\n", (int)buf->st_size);
    printf("Last access: %s",asctime(localtime(&buf->st_atime)));
    printf("Last modification: %s",asctime(localtime(&buf->st_mtime)));
    printf("\n");
}
