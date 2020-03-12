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

void remove_array(struct array_wrapper *ptr){
    if(ptr == NULL)
        return;

    while(--ptr->used >= 0)
        remove_block(ptr->arr[ptr->used]);
    
    free(ptr->arr);
    free(ptr);
}
void compare_files(char *first, char *second, struct array_wrapper *wrapper){
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
        wrapper->arr = realloc(wrapper->arr, (wrapper->used+10)*sizeof(struct block *));
        wrapper->arr_size = wrapper->used+10;
    }
    
    FILE *file = fopen( DIFF_OUT , "r");
    assert(file);

    char *line = NULL;
    size_t len = 0;

    int oper_index = 0;

    while(getline(&line, &len, file) >= 0)
        if(line[0]>='0' && line[0] <= '9')
            oper_index++;
    
    wrapper->arr[wrapper->used++] = create_block(oper_index);
    struct block *b_ptr = wrapper->arr[wrapper->used-1];
    b_ptr->block_size = oper_index;

    
    fclose(file);
    file = fopen( DIFF_OUT , "r");
    size_t oper_len = 0;

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
}