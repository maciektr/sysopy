/**********************************************
    Simple C11 library wrapping system diff. 
**********************************************/

#ifndef _DIFFWRAPPER_H
#define _DIFFWRAPPER_H

#define  _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

    /* Represents single comparison. */
    struct block{
        int block_size;
        int used;
        char **operations;
    };
    struct block *create_block(int size);
    void remove_block(struct block *ptr);

    /* Represents all comparisons. */
    struct array_wrapper{
        int arr_size;
        int used;
        struct block **arr;
    };
    struct array_wrapper *create_array(int size);
    void remove_array(struct array_wrapper *ptr);
    void remove_block_id(int index, struct array_wrapper *ptr);

    /* Reads temp file from diff. */
    struct block *read_block(struct array_wrapper *wrapper);

    /* Compares two specified files (as paths). Returns id of created block.*/
    int make_comparison(char *first, char *second, struct array_wrapper *wrapper);

    /* Compares all files specified by left and right arrays (as paths). */
    void compare_files(int files_count,char **left, char **right, struct array_wrapper *wrapper);

    /* Returns number of operations of block index in array_wrapper ptr. */
    int count_operations(int index, struct array_wrapper *ptr);

    /* Removes operation of index from block ptr. */
    void remove_operation(int index, struct block *ptr);

    /* Counts lines in file specified by path. */
    int count_lines(char *path);
#endif