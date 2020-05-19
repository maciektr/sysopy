#include <linux/limits.h>
#include <sys/resource.h>
#include <stdatomic.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/time.h>
#include <stdbool.h> 
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <time.h> 

#define N_COLORS 256
 
int **read_image(char *filename, int *width, int *height, int *max_color);
void free_img(int **img, int height);
void print_hist(int *hist, char *file_out);
void wait_for_threads(int n_threads, pthread_t *threads);

void *sign_mode_thread(void *args);
void *block_mode_thread(void *args);
void *interleaved_mode_thread(void *args);

// Global variables that are shared within all threads
int img_width, img_height, max_color;
int **image;
int *hist;

typedef struct{
    int th_i;
    int th_n;
    double *retval;
} args_t;

int main(int argc, char *argv[]){
    assert(argc == 5);

    int n_threads = atoi(argv[1]);
    char *mode = argv[2];
    char *file_in = argv[3];
    char *file_out = argv[4];

    pthread_t *threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
    double *rval = (double *)malloc(n_threads * sizeof(double));

    image = read_image(file_in, &img_width, &img_height, &max_color);

    hist = (int *)malloc(N_COLORS*sizeof(int));
    for(int i = 0; i<N_COLORS; i++)
        atomic_init(&hist[i],0);

    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    args_t *args=(args_t *)malloc(n_threads * sizeof(args_t));

    for(int i = 0; i<n_threads; i++){
        args[i].th_n = n_threads;
        args[i].th_i = i;
        args[i].retval = &rval[i];
        if(strcmp(mode, "sign") == 0){
            pthread_create(&threads[i], NULL, sign_mode_thread, (void *)&args[i]);
        }else if(strcmp(mode, "block") == 0){
            pthread_create(&threads[i], NULL, block_mode_thread, (void *)&args[i]);
        }else if(strcmp(mode, "interleaved") == 0){
            pthread_create(&threads[i], NULL, interleaved_mode_thread, (void *)&args[i]);
        }else 
            exit(EXIT_FAILURE);
    }

    wait_for_threads(n_threads, threads);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Execution time: %f s\n", elapsed);
    print_hist(hist, file_out);
    free(hist);
    free(threads);
    free(rval);
    free(args);
    free_img(image, img_height);
}

int **read_image(char *filename, int *width, int *height, int *max_color_val){
    FILE *file = fopen(filename, "r");

    char *line = NULL;
    size_t len = 0; 
    assert(getline(&line, &len, file) != -1);
    assert(strcmp(line, "P2\n") == 0);

    // Read width and height
    while(getline(&line, &len, file) != -1)
        if(line[0] == '#')
            continue;
        else{
            char *pch = strtok(line, " "); 
            assert(pch != NULL);
            *width = atoi(pch);
            pch = strtok(NULL, " ");
            assert(pch != NULL);
            *height = atoi(pch);
            break;
        }

    // Allocate img
    int **img = (int **)malloc((*height) * sizeof(int *));
    for(int i = 0; i<(*height); i++)
        img[i] = (int *)malloc((*width) * sizeof(int));

    // Read max color value
    while(getline(&line, &len, file) != -1)
        if(line[0] == '#')
            continue;
        else{
            char *pch = strtok(line, " "); 
            assert(pch != NULL);
            *max_color_val = atoi(pch);
            break;
        }

    // Read img
    int w=0,h=0;
    while(getline(&line, &len, file) != -1){
        if(h >= img_height)
            break;
        if(line[0] == '#')
            continue;
        else{
            char *pch = strtok(line, " ");
            while(pch != NULL){
                if(w >= img_width)
                    break;
                img[h][w] = atoi(pch);
                w++;
                pch = strtok(NULL," ");
            }
        }
        w = 0;
        h++;
    }

    free(line);
    fclose(file);
    return img;
}

void free_img(int **img, int height){
    while(--height >= 0)
        free(img[height]);
    free(img);
}

void print_hist(int *histogram, char *file_out){
    FILE *out = NULL;
    if(file_out != NULL)
        out = fopen(file_out, "w");
    
    for(int c = 0; c<N_COLORS; c++){
        printf("%d: %d\n", c, atomic_load(&histogram[c]));
        if(out != NULL)
            fprintf(out,"%d: %d\n", c, atomic_load(&histogram[c]));
    }
    if(out != NULL)
        fclose(out);
}

void wait_for_threads(int n_threads, pthread_t *threads){
    void *rval_ptr=NULL;
    while(--n_threads >= 0){
        pthread_join(threads[n_threads],&rval_ptr);
        printf("Thread %d took %f seconds.\n", n_threads, (double)(*((double *)rval_ptr)));
    }
}

void *sign_mode_thread(void *args){
    args_t *args_ = (args_t *)args;
    struct timespec start, finish;
    clock_gettime(_POSIX_MONOTONIC_CLOCK, &start);

    for(int i = 0; i<img_height; i++)
        for(int k = 0; k<img_width; k++)
            if((image[i][k] % args_->th_n) == (args_->th_i))
                __atomic_fetch_add(&hist[image[i][k]], 1, __ATOMIC_RELAXED);

    clock_gettime(_POSIX_MONOTONIC_CLOCK, &finish);
    *(args_->retval) = (finish.tv_sec - start.tv_sec);
    *(args_->retval) += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    pthread_exit(args_->retval);
}

void *block_mode_thread(void *args){
    args_t *args_ = (args_t *)args;
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int low = args_->th_i * ((img_width / args_->th_n) + (img_width % args_->th_n == 0 ? 0 : 1));
    int high = (args_->th_i + 1) * ((img_width / args_->th_n) + (img_width % args_->th_n == 0 ? 0 : 1));
    for(int i = low; i<high && i<img_width; i++)
        for(int k = 0; k<img_height; k++)
            __atomic_fetch_add(&hist[image[i][k]], 1, __ATOMIC_RELAXED);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    *(args_->retval) = (finish.tv_sec - start.tv_sec);
    *(args_->retval) += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    pthread_exit(args_->retval);
}

void *interleaved_mode_thread(void *args){
    args_t *args_ = (args_t *)args;
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int k = args_->th_i;
    while(k < img_width){
        for(int i = 0; i<img_height; i++)
            __atomic_fetch_add(&hist[image[i][k]], 1, __ATOMIC_RELAXED);
        k += args_->th_n;
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
    *(args_->retval) = (finish.tv_sec - start.tv_sec);
    *(args_->retval) += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    pthread_exit(args_->retval);
}