#include <stdio.h>
#include <time.h>

#include "../Z1/diffwrapper.h"

#define TIME_RES "time.txt"

int main(int argc, char **argv){
    struct array_wrapper *array = NULL;
    FILE *file = fopen(TIME_RES, "a");
    assert(file);
    
    fprintf(file, "Command: ");
    for(int i = 0; i<argc; i++)
        fprintf(file,"%s ", argv[i]);
    fprintf(file, "\n");

    clock_t begin = clock();

    int n = 1;
    while(n < argc){
        if(strcmp(argv[n], "create_table") == 0){
            array = create_array(atoi(argv[n+1]));
            n+=2;
        }else if(strcmp(argv[n], "compare_pairs") == 0){
            while(++n < argc && strstr(argv[n], ":") ){
                char *first = strtok(argv[n], ":");
                char *second = strtok(NULL, ":");
                make_comparison(first, second, array);
            }

        }else if(strcmp(argv[n], "count_operations") == 0){
            printf("%d\n" ,count_operations(atoi(argv[n+1]), array));
            n+=2;
        }else if(strcmp(argv[n], "remove_block") == 0){
            remove_block_id(atoi(argv[n+1]), array);
            n+=2;
        }else if(strcmp(argv[n], "remove_operation") == 0){
            remove_operation(atoi(argv[n+2]), array->arr[atoi(argv[n+1])]);
            n+=3;
        }
    }
    remove_array(array);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Took: %f seconds.\n", time_spent);
    fprintf(file, "Took: %f seconds.\n", time_spent);
    fclose(file);

    return 0;
}