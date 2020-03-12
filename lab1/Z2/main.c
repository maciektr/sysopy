#include<stdio.h>

#include "../Z1/diffwrapper.h"

int main(){
    struct array_wrapper *ptr = create_array(10);
    compare_files("a.txt", "b.txt", ptr);
    remove_array(ptr);
    return 0;
}