#include <sys/types.h> 
#include <sys/stat.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h> 
#include <stdio.h>
#include <fcntl.h> 

char get_char(int index){
    const int N_LATIN = (int)'Z' - (int)'A';
    const int N_CHARS=(2*N_LATIN)+9;
    index = abs(index);
    index %= N_CHARS;
    char c = (char)((int)'0'+index-2*N_LATIN);

    if(index <= N_LATIN)
        c = (char)((int)'A'+index);
    else if(index <= 2*N_LATIN)
        c = (char)((int)'a'+index-N_LATIN);

    // printf("%d = %c\n",index, c);
    return c;
}

void generate(char *name, int n, int size){
        int r = open("/dev/random", O_RDONLY);
        assert(r >= 0);
        int f = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        assert(f >= 0);

        char *buf = malloc(size);
        while(n--){
            assert(read(r, buf, size) >= 0);
            for(int i = 0; i<size; i++){
                *(buf+i) = get_char((int)*(buf+i));
            }
            // *(buf + size-2) ='\n';
            // *(buf + size-1) ='\0';
            assert(write(f,buf, size) >= 0);
        }
        free(buf);
        close(f);
        close(r);        
}

void copy(char *first, char *second, int n, int size, bool sys){
    if(sys){
        int from = open(first, O_RDONLY);
        int to = open(second, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        char *buf = malloc(size);

        while(n--){
            assert(read(from, buf, size) >= 0);
            assert(write(to, buf, size) >= 0);
        }

        free(buf);
        close(from);
        close(to);
    }else{
        FILE *from = fopen(first, "r");
        FILE *to = fopen(second, "w");
        char *buf = malloc(size);

        while(n--){
            assert(fread(buf, sizeof(char), size, from) >= 0);
            assert(fwrite(buf, sizeof(char), size, to) >= 0);
        }

        free(buf);
        fclose(from);
        fclose(to);
    }
}

void _sort(char *name, int p, int q, int size, bool sys){
    if(p>=q)
        return;
    char *pivot = malloc(size*sizeof(char));
    if(sys){
        int file = open(name, O_RDWR);
        assert(lseek(file, p*size, SEEK_SET)>=0);
        assert(read(file, pivot, size) >= 0);
        
        int k = p+1;
        char *buf = malloc(size);
        for(int i = p+1; i<=q; i++){
            assert(lseek(file, i*size, SEEK_SET) >= 0);
            assert(read(file, buf, size) >= 0);
            // i <= pivot
            if(strncmp(buf, pivot,size) < 0){
                if(i==k){
                    k++;
                    continue;
                }
                // Swap i,k
                char *swap_buf = malloc(size);
                assert(lseek(file, k*size, SEEK_SET) >= 0);
                assert(read(file, swap_buf, size) >= 0);
                assert(lseek(file, k*size, SEEK_SET) >= 0);
                assert(write(file, buf, size) >= 0);
                assert(lseek(file, i*size, SEEK_SET) >= 0);
                assert(write(file, swap_buf, size) >= 0);
                free(swap_buf);
                k++;    
            }
        }
        k--;
        if(k > p){
            char *swap_buf = malloc(size);
            assert(lseek(file, k*size, SEEK_SET) >= 0);
            assert(read(file, swap_buf, size) >= 0);
            assert(lseek(file, k*size, SEEK_SET) >= 0);
            assert(write(file, pivot, size) >= 0);
            assert(lseek(file, p*size, SEEK_SET) >= 0);
            assert(write(file, swap_buf, size) >= 0);
            free(swap_buf);
        }
        free(buf);
        close(file);
        _sort(name, p, k-1, size, sys);
        _sort(name, k+1, q, size, sys);
    }else{
        FILE *file = fopen(name, "r+");
        assert(fseek(file, p*size, 0) >= 0);
        assert(fread(pivot, sizeof(char), size, file) >= 0);
        int k = p+1;

        char *buf = malloc(size*sizeof(char));
        for(int i = p+1; i<=q; i++){
            assert(fseek(file, i*size, 0) >= 0);
            assert(fread(buf, sizeof(char), size, file) >= 0);
            // i <= pivot
            if(strncmp(buf, pivot, size) < 0){
                if(i==k){
                    k++;
                    continue;
                }
                // Swap i,k
                char *swap_buf = malloc(size);
                assert(fseek(file, k*size, 0) >= 0);
                assert(fread(swap_buf, sizeof(char), size, file) >= 0);
                assert(fseek(file, k*size, 0) >= 0);
                assert(fwrite(buf, sizeof(char), size, file) >= 0);
                assert(fseek(file, i*size, 0) >= 0);
                assert(fwrite(swap_buf, sizeof(char), size, file) >= 0);
                free(swap_buf);
                k++;    
            }
        }
        k--;
        if(k > p){
            char *swap_buf = malloc(size);
            assert(fseek(file, k*size, 0) >= 0);
            assert(fread(swap_buf, sizeof(char), size, file) >= 0);
            assert(fseek(file, k*size, 0) >= 0);
            assert(fwrite(pivot, sizeof(char), size, file) >= 0);
            assert(fseek(file, p*size, 0) >= 0);
            assert(fwrite(swap_buf, sizeof(char), size, file) >= 0);
            free(swap_buf);
        }

        free(buf);
        fclose(file);
        _sort(name, p, k-1, size, sys);
        _sort(name, k+1, q, size, sys);
    }
    free(pivot);        
}

void sort(char *name, int n, int size, bool sys){
    _sort(name, 0, n-1, size, sys);    
}

int main(int argc, char *argv[]){
    if(argc <= 1)
        return 0;
    if(strcmp(argv[1], "generate") == 0){   
        generate(argv[2], atoi(argv[3]), atoi(argv[4]));
    }else if(strcmp(argv[1], "sort")==0){
        bool sys = strcmp(argv[5],"sys") == 0;
        sort(argv[2], atoi(argv[3]), atoi(argv[4]), sys);
    }else if(strcmp(argv[1], "copy")==0){
        bool sys = strcmp(argv[6],"sys") == 0;
        copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), sys);
    }
    return 0;
}