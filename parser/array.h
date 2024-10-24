// 
//    Quick and efficient header only dynamic array implementation:
//
//      Either create an array with array_new() OR array_new_with_capacity(type, amount)
// 
//      Add to array with array_push(array, item_to_add) or array_push_ptr(array, ptr_to_data, size_of_data_in_bytes)
//      Etc, not that complicated
// 




#pragma once
#include "defs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define array_new_with_capacity(TYPE, AMOUNT) _array_new_with_capacity(sizeof(TYPE) * (AMOUNT))
#define array_push(ARRAY, ITEM) array_push_ptr((ARRAY), ((void*)&((ITEM))), sizeof((ITEM)))
#define array_is_empty(ARRAY) ((ARRAY)->ptr == NULL || (ARRAY)->size == 0)
#define array_last_item_ptr(ARRAY, TYPE) (TYPE*)_array_last_item_ptr((ARRAY), sizeof(TYPE))
#define array_last_item(ARRAY, TYPE) *array_last_item_ptr((ARRAY), TYPE)

// WARNING: Use of popped value after a push is UNDEFINED BEHAVIOR!
#define array_pop_items_ptr(ARRAY, TYPE, AMOUNT) _array_pop_item((ARRAY), sizeof(TYPE) * (AMOUNT))
#define array_pop_item(ARRAY, TYPE) *(TYPE*) array_pop_items_ptr((ARRAY), TYPE, 1)

typedef struct {
    void* ptr;
    size_t size;
    size_t capacity;
} Array;

static inline Array array_new() {
    return (Array){
        NULL,
        0,
        0
    };
}

static inline Array _array_new_with_capacity(size_t capacity) {
    Array ret = {
        malloc(capacity),
        0,
        capacity
    };
    debug_assert(ret.ptr != 0);
    return ret;
}

static inline void array_grow_for(Array* array, size_t size){
    if (array->size >= size) return;


    // EXPONENTIAL GROWTH!
    size_t grown_cap = (array->capacity + 1) << 1;
    array->capacity = grown_cap > size ? grown_cap : size;
    array->capacity = array->capacity > 1024 ? array->capacity : 1024;
    debug_assert(array->capacity != 0);

    array->ptr = realloc(array->ptr, array->capacity);
    debug_assert(array->ptr != NULL);
    
}

// O(M) where M is size
static inline void array_push_ptr(Array* array, const void* restrict item, size_t size) {
    size_t new_size = array->size + size;
    array_grow_for(array, new_size);

    memcpy((uint8_t*) array->ptr + array->size, item, size);
    
    array->size = new_size;
}
static inline void array_free(Array* array){ if (array->ptr) free(array->ptr); }
static inline void array_push_from_other(Array* restrict array, Array* restrict other){
    debug_assert(other);
    
    if (array_is_empty(other)) return;
    array_push_ptr(array, other->ptr, other->size);
}


static inline void* _array_last_item_ptr(Array* array, size_t size) {
    if (array_is_empty(array)) return NULL;
    return (uint8_t*) array->ptr + array->size - size;
}

static inline void* _array_pop_item(Array* array, size_t size) {
    if (array->size < size) return NULL;
    array->size -= size;
    return (uint8_t*) array->ptr + array->size;
}


// Puts a null after the end of an array. So if the array is of chars
// this makes it a valid string. (Be aware ANY PUSH writes over this!)
//
// DO NOT STORE THE PTR FOR LONG TERM USE! CLONE THE STRING FOR THAT!
//
// Return: The internal ptr
static inline char* array_force_null_terminate(Array* array){
    // Ensure space
    array_grow_for(array, array->size + 1);

    ( (char*)array->ptr )[array->size] = 0;

    return array->ptr;
}

