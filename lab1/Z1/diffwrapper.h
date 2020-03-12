#ifndef _DIFFWRAPPER_H
#define _DIFFWRAPPER_H

#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>


    struct block{
        int block_size;
        int used;
        char **operations;
    };

    void remove_block(struct block *ptr);

    struct array_wrapper{
        int arr_size;
        int used;
        struct block **arr;
    };

    struct array_wrapper *create_array(int size);
    void remove_array(struct array_wrapper *ptr);

    void compare_files(char *first, char *second, struct array_wrapper *wrapper);

    // void remove_block(int block_index);
    void remove_change(int block_index, int change_index);

    int count_changes(int block_index);
    
#endif