#include "error.h"

#include "ANSI-color-codes.h"
#include <string.h>
#include "array.h"


static void _add_to_in_error_mode(Array* array, const char* input, size_t len){
    if (is_terminal()){
        array_push_ptr(array, REDHB, sizeof(REDHB));
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







