#include <assert.h>
#include <stdio.h>
#include "common/array.h"

void test_array_new() {
    Array arr = array_new();
    assert(array_is_empty(&arr));
    assert(arr.capacity == 0);
    array_free(&arr);
}

void test_array_push() {
    Array arr = array_new_with_capacity(int, 5);
    int item = 42;
    array_push(&arr, item);

    assert(arr.size == sizeof(int));
    assert(array_last_item(&arr, int) == 42);
    array_free(&arr);
}

void test_array_pop() {
    Array arr = array_new_with_capacity(int, 5);
    int item1 = 42;
    int item2 = 84;
    array_push(&arr, item1);
    array_push(&arr, item2);

    int popped_item = array_pop_item(&arr, int);
    assert(popped_item == 84);
    assert(arr.size == sizeof(int));
    
    popped_item = array_pop_item(&arr, int);
    assert(popped_item == 42);
    assert(array_is_empty(&arr));
    
    array_free(&arr);
}

void test_array_push_from_other() {
    Array arr1 = array_new_with_capacity(int, 5);
    Array arr2 = array_new_with_capacity(int, 5);
    
    int item1 = 1;
    int item2 = 2;
    array_push(&arr1, item1);
    array_push(&arr1, item2);
    
    array_push_from_other(&arr2, &arr1);
    
    assert(arr2.size == arr1.size);
    assert(array_last_item(&arr2, int) == 2);
    
    array_free(&arr1);
    array_free(&arr2);
}

int main() {
    test_array_new();
    test_array_push();
    test_array_pop();
    test_array_push_from_other();


    Array x = array_new();
    uint32_t xx = 2;
    array_push(&x, xx);
    array_push_ptr(&x, calloc(1, 1024), 1024);
    assert(x.size == 1028);
    assert(0 == array_pop_item(&x, uint32_t));
    assert(x.size == 1024);
    
    return 0;
}
