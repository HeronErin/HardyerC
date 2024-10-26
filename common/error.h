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


#define is_terminal() isatty(STDERR_FILENO)


void raise_context_less_error(const char* input, int status);