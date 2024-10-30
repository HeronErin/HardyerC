#include "error.h"

#include <string.h>
#include "array.h"


const char* ce_to_str(CompilationError ce){
    switch (ce){
        case ERR_NUM_PERIODS: return "Invalid amount of periods in number";
        case ERR_INVALID_NUMBER_PREFIX: return "Prefix not valid in number";
        case ERR_NUMBER_INTERLACE: return "Invalid char found in number";
        default: return "UNKNOW ERROR";
    }
}



static void _add_to_in_error_mode(Array* array, const char* input, size_t len){
    if (is_terminal()){
        array_push_ptr(array, ERROR_CODES, sizeof(ERROR_CODES));
        array_push_ptr(array, input, len);
        array_push_ptr(array, reset, sizeof(reset));
    }else{
        array_push_ptr(array, input, len);
    }
}
static void add_to_in_error_mode(Array* array, const char* input){
    _add_to_in_error_mode(array, input, strlen(input));
}



void raise_context_less_error(const char* input, int status){
    Array output_buffer = array_new();

    add_to_in_error_mode(&output_buffer, "ERROR");
    array_push(&output_buffer, ": ");
    array_push_ptr(&output_buffer, input, strlen(input));
    array_push(&output_buffer, "\n\n");


    array_force_null_terminate(&output_buffer);
    
    write(STDERR_FILENO, output_buffer.ptr, output_buffer.size);

    exit(status);
}

void raise_error_with_context(TranslationParserState* tsc, uint16_t origin, uint16_t index_into_origin, const char* error_message, int status){
    // TODO: ALL OF THIS

    raise_context_less_error(error_message, status);
}





