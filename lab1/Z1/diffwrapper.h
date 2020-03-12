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
    struct block *create_block(int size);
    void remove_block(struct block *ptr);

    struct array_wrapper{
        int arr_size;
        int used;
        struct block **arr;
    };
    struct array_wrapper *create_array(int size);
    void remove_array(struct array_wrapper *ptr);
    void remove_block_id(int index, struct array_wrapper *ptr);

    struct block *read_block(struct array_wrapper *wrapper);
    int make_comparison(char *first, char *second, struct array_wrapper *wrapper);
    void compare_files(int files_count,char **left, char **right, struct array_wrapper *wrapper);
    int count_operations(int index, struct array_wrapper *ptr);
    void remove_operation(int index, struct block *ptr);

    // helper functions
    int count_lines(char *path);
#endif