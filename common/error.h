// Compilation error handling system:
//
//    No compilation error can be recovered from, and are rare
//  therefore there is no need to make it fast, only verbose.
//  
//    Errors are formatted in two ways, tty mode, and pipe mode.
//  If you are in a tty, then ansi codes are used to make errors
//  more readable. Otherwise we output things without colors, that
//  way it can potentially be parsed easier. 

#pragma once


#include <unistd.h>
#include "array.h"


#include "ANSI-color-codes.h"

#include "parser/state.h"

#include <stdio.h>

#define ERROR_CODES BLINK REDHB

#define is_terminal() isatty(STDERR_FILENO)


void raise_context_less_error(const char* error_message, int status);
void raise_error_with_context(TranslationParserState* tsc, uint16_t origin, uint16_t index_into_origin, const char* error_message, int status);

typedef enum {
    ERR_UNKNOWN,
    ERR_UNEXPECTED_EOF,
    ERR_INVALID_NUMBER_PREFIX,
    ERR_INVALID_NUMBER_SUFFIX,
    ERR_NUMBER_INTERLACE,
    ERR_NUM_PERIODS,
    ERR_NUM_BASE,

    ERR_UNEXPECTED_EOL,
} CompilationError;


const char* ce_to_str(CompilationError ce);

// Error some type. Useful for when you do not wish to provide a function with all the context just for errors
typedef struct{
    char is_error;
    union{
        void* proper_result;
        struct{
            uint16_t error_length;
            
            CompilationError err;
            char* invalid_code_ptr;
            
            // Text explaining error (if heap WILL BE LEAKED)
            const char* error_text;
        };
    };

} FalliblePtrResult;

static inline FalliblePtrResult FPR_ok(void* ok){
    FalliblePtrResult res;
    res.is_error = 0;
    res.proper_result = ok;
    return res;
}

static inline FalliblePtrResult FPR_err(char* invalid_code_ptr, uint16_t error_length, CompilationError err, char* error_text){
    FalliblePtrResult res;
    res.is_error = 1;
    res.invalid_code_ptr = invalid_code_ptr;
    res.error_length = error_length;
    res.err = err;
    res.error_text = error_text;
    return res;
}

static inline void FPR_debug(FalliblePtrResult fpr){
    if (!fpr.is_error)
        printf("<FPR: RES=0x%lx>", (size_t) fpr.proper_result);
    else
        printf("<FPR: ERR=%s:%s>", ce_to_str(fpr.err), fpr.error_text ? fpr.error_text : "");
    
}


// Get the index in the string relative to the origin param.
// UNDEFINED BEHAVIOR FOR NON ERROR TYPES!
static inline size_t FPR_make_index(FalliblePtrResult fpr, char* start_of_str){
    debug_assert(fpr.is_error);
    debug_assert(fpr.invalid_code_ptr >= start_of_str);

    return fpr.invalid_code_ptr - start_of_str;
}


static inline void* FPR_unwrap(FalliblePtrResult fpr, TranslationParserState* tsc, uint16_t origin){
    if (!fpr.is_error) return fpr.proper_result;
    
    raise_error_with_context(
        tsc, 
        origin, 
        FPR_make_index(
            fpr, 
            ps_temporary_view(
                &((struct LoadedFile*) tsc->loaded_files.ptr)[origin].contents
            )
        ), 
        ce_to_str(fpr.err), 
        -1
        );
}
static inline void* FPR_unwrap_unchecked(FalliblePtrResult fpr) {
    debug_assert(!fpr.is_error);
    return fpr.proper_result;
}
