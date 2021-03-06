#include "diffwrapper.h"
#define DIFF_CMD "diff "
#define DIFF_OUT "diff_out.txt"
#define SPACE " "


struct array_wrapper *create_array(int size){
    struct array_wrapper *ptr = malloc(sizeof(struct array_wrapper));
    ptr->used = 0;
    ptr->arr = malloc(size * sizeof(struct block *));
    ptr->arr_size = size;
    return ptr;
}

struct block *create_block(int size){
    struct block *ptr = malloc(sizeof(struct block));
    ptr->used = 0;
    ptr->operations = malloc(size*sizeof(char *));
    ptr->block_size = size;
    return ptr;
}

void remove_block(struct block *ptr){
    if(ptr == NULL)
        return;
    
    while(--ptr->used >= 0)
        free(ptr->operations[ptr->used]);

    free(ptr->operations);
    free(ptr);
}

void remove_block_id(int index, struct array_wrapper *ptr){
    if(ptr == NULL || index >= ptr->used)
        return;
    remove_block(ptr->arr[index]);
    ptr->arr[index] = NULL;
    
    if(index == ptr->used-1)
        ptr->used--;
}

void remove_array(struct array_wrapper *ptr){
    if(ptr == NULL)
        return;

    while(--ptr->used >= 0)
        remove_block(ptr->arr[ptr->used]);
    
    free(ptr->arr);
    free(ptr);
}

int count_lines(char *path){
    FILE *file = fopen( path , "r");
    assert(file);
    
    int result = 0;
    char *line = NULL;
    size_t len = 0;

    while(getline(&line, &len, file) >= 0)
        if(line[0]>='0' && line[0] <= '9')
            result++;

    free(line);
    fclose(file);
    return result;
}

struct block *read_block(struct array_wrapper *wrapper){
    int oper_index = count_lines(DIFF_OUT);
    struct block *b_ptr = create_block(oper_index);
    b_ptr->block_size = oper_index;
    
    FILE *file = fopen( DIFF_OUT , "r");
    assert(file);

    size_t oper_len = 0;
    char *line = NULL;
    size_t len = 0;

    while(getline(&line, &len, file) >= 0){
        if(line[0]>='0' && line[0] <= '9'){
            if(oper_len != 0)
                b_ptr->operations[b_ptr->used++] = calloc((oper_len+1), sizeof(char));
            oper_len = 0;
        }
        oper_len+=len;
    }
    b_ptr->operations[b_ptr->used++] = calloc((oper_len+1), sizeof(char));
    
    fclose(file);
    file = fopen(DIFF_OUT , "r");
    oper_index = -1;
    
    while(getline(&line, &len, file) >= 0)
        if(line[0]>='0' && line[0] <= '9')
            strcpy(b_ptr->operations[++oper_index], line);
        else
            strcat(b_ptr->operations[oper_index], line);

    fclose(file);
    free(line);
    return b_ptr;
}

int make_comparison(char *first, char *second, struct array_wrapper *wrapper){
    char *command = calloc((strlen(DIFF_CMD)+strlen(first)+strlen(SPACE)+strlen(second)+3+strlen(DIFF_OUT)), sizeof(char));

    strcpy(command, DIFF_CMD);
    strcat(command, first);
    strcat(command, SPACE);
    strcat(command, second);
    strcat(command, " > ");
    strcat(command, DIFF_OUT);

    system(command);
    free(command);

    if(wrapper->used >= wrapper->arr_size){
        wrapper->arr = realloc(wrapper->arr, (2*wrapper->used)*sizeof(struct block *));
        wrapper->arr_size = 2*wrapper->used;
    }

    wrapper->arr[wrapper->used++] = read_block(wrapper);   
    system("rm diff_out.txt");

    return wrapper->used-1;
}

void compare_files(int files_count,char **left, char **right, struct array_wrapper *wrapper){
    for(int i = 0; i<files_count; i++){
        make_comparison(left[i], right[i], wrapper);
    }
}

int count_operations(int index, struct array_wrapper *ptr){
    assert(ptr->arr[index]);
    return ptr->arr[index]->used;
}

void remove_operation(int index, struct block *ptr){
    if(ptr == NULL || index >= ptr->used)
        return;
    free(ptr->operations[index]);
    while(index + 1 < ptr->used){
        ptr->operations[index] = ptr->operations[index+1];
        index++;
    }
    ptr->used--;
}