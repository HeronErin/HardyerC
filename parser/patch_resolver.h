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
    return (PatchString){
        array_new(),
        array_new()
    };
}

// O(N)
static inline PatchString ps_from(char* input){
    debug_assert(input != NULL);

    size_t len = strlen(input);
    
    Array str = _array_new_with_capacity(len);
    array_push_ptr(&str, input, len);
     
    return (PatchString){
        str,
        array_new()
    };
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
static inline void ps_insert_at_end(PatchString* restrict ps, char* str_to_insert){
    debug_assert(str_to_insert != NULL);

    const size_t old_size = ps->internal_string.size;
    const size_t addition_size = strlen(str_to_insert);
    
    array_push_ptr(&ps->internal_string, str_to_insert, addition_size);
    array_push(&ps->patches, ((struct _Patch){
        old_size,
        old_size,
        addition_size
    }));
}


// O(N+M) where M is strlen to insert, and N is remainder of internal str after point (If within capacity)
// Returns: True if successful, false otherwise
//
// WARNING: THIS IS SLOW
bool ps_insert_at(PatchString* restrict ps, size_t point, char* str_to_insert);

// O(N) where N is remainder of internal str after point. Due to the nature of deletion
// no realloc is used. Therefore the O(N) is guaranteed
//
// WARNING: THIS IS SLOW
bool ps_delete(PatchString* restrict ps, size_t point, size_t amount);



// Both these functions are O(N) where N is the amount of patches.
//
// Their purpose is to use the logged patches applied to the string to convert to/for
// an estimated location in the original/final string. The purpose of this is primarily
// to determine where to underline errors, and for what to define __line__ as.
size_t ps_to_original_index(const PatchString* restrict ps, size_t index);
size_t ps_to_modified_index(const PatchString* restrict ps, size_t index);




typedef enum{
    // Uses: nothing
    CR_KEEP = 0,
    
    // Uses: amount
    CR_DISCARD,
    // Uses: amount, insertion
    CR_DISCARD_AND_INSERT,
    CR_DISCARD_AND_INSERT_CHAR,

    // Insert then free the string
    // Uses: amount, insertion
    CR_DISCARD_AND_INSERT_OWNED
} ConstructionResultVariety;

typedef struct{
    ConstructionResultVariety variety;

    const char* insertion; // Null for deletions
    size_t amount; // Amount of space to delete (not used in insertions)


    char insertion_char; // Only used for CR_DISCARD_AND_INSERT_CHAR
    
} ConstructionResult;


static const ConstructionResult CR_NOP = {0};

// This function pointer is called during ps construction. And the window will never contain
// null within the window. Called once before iteration starts (with window ptr set to NULL!)
// it is suggested to use static variables for state. During initialization return is IGNORED
//
// Returns: ConstructionResult
//
// Param: Window ptr(NULL to specify initialization)
typedef ConstructionResult (*WindowPredicate)(const char* restrict );


PatchString _ps_windowed_construction(
        const char* restrict source, 
        size_t source_len, 
        uint8_t window_size, 
        const WindowPredicate predicate);


void ps_reconstruct_using(PatchString* ps, uint8_t window_size, const WindowPredicate predicate);

static inline PatchString ps_construct_from(const char* source, uint8_t window_size, const WindowPredicate predicate){
    return _ps_windowed_construction(
        source,
        strlen(source),
        window_size,
        predicate
    );
}





