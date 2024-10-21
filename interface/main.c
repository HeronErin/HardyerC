#include <stdio.h>
#include "parser/array.h"

int main(){
    Array x = array_new();
    int xx = 2;
    array_push(&x, xx);
    array_push_ptr(&x, calloc(1, 1024), 1024);
    

    printf("Test %i\n", array_pop_item(&x, int));

    printf("Test %i\n", array_pop_item(&x, int));
    
}