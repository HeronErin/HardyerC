#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define array_new_with_capacity(TYPE, AMOUNT) _array_new_with_capacity(sizeof(TYPE) * (AMOUNT))
#define array_push(ARRAY, ITEM) array_push_ptr((ARRAY), ((void*)&(ITEM)), sizeof(ITEM))
#define array_is_empty(ARRAY) ((ARRAY).ptr == NULL && (ARRAY).size == 0)
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
    Array ret = {
        NULL,
        0,
        0
    };
    return ret;
}

static inline Array _array_new_with_capacity(size_t capacity) {
    Array ret = {
        malloc(capacity),
        0,
        capacity
    };
    return ret;
}

static inline void array_push_ptr(Array* array, void* item, size_t size) {
    size_t new_size = array->size + size;
    

    // EXPONENTIAL GROWTH!
    size_t grown_cap = (array->capacity + 1) << 1;
    array->capacity = grown_cap > size ? grown_cap : size;


    array->ptr = realloc(array->ptr, array->capacity);

    memcpy((uint8_t*) array->ptr + array->size, item, size);
    array->size = new_size;
}
static inline void array_free(Array* array){ free(array->ptr); }
static inline void array_push_from_other(Array* array, Array* other){
    if (array_is_empty(*other)) return;
    array_push_ptr(array, other->ptr, other->size);
}


static inline void* _array_last_item_ptr(Array* array, size_t size) {
    if (array_is_empty(*array)) return NULL;
    return (uint8_t*) array->ptr + array->size - size;
}

static inline void* _array_pop_item(Array* array, size_t size) {
    if (array->size < size) return NULL;
    array->size -= size;
    return (uint8_t*) array->ptr + array->size;
}
