#pragma once
#include "defs.h"
#include "array.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

struct _Patch{
    size_t start;
    size_t end;
    ptrdiff_t len_mod;
};

typedef struct{
    Array internal_string; // Array type: int8_t NOT NULL TERMINATED!
    Array patches; // Array type: struct _Patch
} PatchString;

static inline void ps_free(PatchString* ps){
    array_free(&ps->internal_string);
    array_free(&ps->patches);
}

static inline PatchString ps_new(){
    PatchString ps = {
        array_new(),
        array_new()
    };
    return ps;
}

// O(N)
static inline PatchString ps_from(char* input){
    debug_assert(input != NULL);

    size_t len = strlen(input);
    Array str = _array_new_with_capacity(len);
    array_push_ptr(&str, input, len);
    PatchString ps = {
        str,
        array_new()
    };
    return ps;
}

// O(N)
static inline char* ps_clone_str(PatchString* ps){
    size_t size = ps->internal_string.size;
    // Size + null
    char* str = malloc(size + 1);

    memcpy(str, ps->internal_string.ptr, size);
    
    // Add in the null
    str[size + 1] = 0;
    return str;
}


// O(M) where M is strlen to insert (If within capacity)
static inline void ps_insert_at_end(PatchString* ps, char* str_to_insert){
    debug_assert(str_to_insert != NULL);

    const size_t old_size = ps->internal_string.size;
    const size_t addition_size = strlen(str_to_insert);
    struct _Patch insertion = {
        old_size,
        old_size,
        addition_size
    };
    
    array_push_ptr(&ps->internal_string, str_to_insert, addition_size);
    array_push(&ps->internal_string, insertion);
}

// O(N+M) where M is strlen to insert, and N is remainder of internal str after point (If within capacity)
// Returns: True if successful, false otherwise
static inline bool ps_insert_at(PatchString* ps, size_t point, char* str_to_insert){
    debug_assert(str_to_insert != NULL);
    
    const size_t old_size = ps->internal_string.size;
    const size_t addition_size = strlen(str_to_insert);

    debug_assert(point <= old_size);
  

}

