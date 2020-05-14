#include <linux/limits.h>
#include <sys/resource.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/time.h>
#include <stdbool.h> 
#include <stdlib.h>
#include<pthread.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <time.h> 

int **read_image(char *filename, int *width, int *height);
void free_img(int **img, int height);

void *sign_mode_thread(void *args);
void *block_mode_thread(void *args);
void *interleaved_mode_thread(void *args);

typedef struct{

} args_t;

int main(int argc, char *argv[]){
    assert(argc == 5);

    int n_threads = atoi(argv[1]);
    char *mode = argv[2];
    char *file_in = argv[3];
    char *file_out = argv[4];

    pthread_t *threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));

    int img_width, img_height;
    int ** image = read_image(file_in, &img_width, &img_height);

    args_t args;

    for(int i = 0; i<n_threads; i++)
        if(strcmp(mode, "sign") == 0){
            pthread_create(&threads[i], NULL, sign_mode_thread, (void *)&args);
        }else if(strcmp(mode, "block") == 0){
            pthread_create(&threads[i], NULL, block_mode_thread, (void *)&args);
        }else if(strcmp(mode, "interleaved") == 0){
            pthread_create(&threads[i], NULL, interleaved_mode_thread, (void *)&args);
        }else 
            exit(EXIT_FAILURE);

    free_img(image, img_height);
}

int **read_image(char *filename, int *width, int *height){
    FILE *file = fopen(filename, "r");

    char *line = NULL;
    size_t len = 0; 
    assert(getline(&line, &len, file) != -1);
    assert(strcmp(line, "P2") == 0);

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

    // Ignore the one line with max gray val
    while(getline(&line, &len, file) != -1)
        if(line[0] == '#')
            continue;
        else
            break;

    // Read img
    int w=0,h=0;
    while(getline(&line, &len, file) != -1){
        if(line[0] == '#')
            continue;
        else{
            char *pch = strtok(line, " ");
            while(pch != NULL){
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

void *sign_mode_thread(void *args){

}

void *block_mode_thread(void *args){

}

void *interleaved_mode_thread(void *args){

}