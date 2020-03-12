#include<stdio.h>

#include "../Z1/diffwrapper.h"

int main(){
    struct array_wrapper *ptr = create_array(10);

    // make_comparison("a.txt", "b.txt", ptr);
    char *left[] = {"a.txt", "b.txt"};
    char *right[] = {"b.txt", "a.txt"};
    compare_files(2,left, right, ptr);
    printf("%d\n",count_operations(0, ptr));
    remove_operation(1,ptr->arr[0]);

    remove_array(ptr);
    return 0;
}