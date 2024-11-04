#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "number_parsing.h"

// Type catagories of tokens exist:
// 
// token:
//         keyword
//         identifier
//         constant
//         string-literal
//         operator
//         punctuator

// preprocessing-token:
//         header-name
//         identifier
//         pp-number
//         character-constant
//         string-literal
//         operator
//         punctuator
//         each non-white-space character that cannot be one of
//         the above

static inline bool is_punctuator_single(const char c){
    switch (c){
        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case '*':
        case ',':
        case ':':
        case '=':
        case ';':
        case '#':
        case '\'':
            return true;
        default:
    }
    return false;
}




typedef enum{
    TT_UNKNOWN,
    TT_KEYWORD,
    TT_IDENTIFIER,
    TT_CONSTANT,
    TT_STRING_LITERAL,
    TT_OPERATOR,
    TT_PUNCTUATOR,

    

    // Bit 16 signifies pp tokens
    TTPP = 1 << 16,
    TTPP_UNKNOWN = 1 << 16,
    TTPP_HEADER_NAME,
    TTPP_IDENTIFIER,
    TTPP_PP_NUMBER,
    TTPP_CHAR_CONSTANT,
    TTPP_STRING_LITERAL,
    TTPP_OPERATOR,
    TTPP_PUNCTUATOR,    
} TokenType;

typedef struct{
    TokenType type;
    
    // Index AFTER initial translation phases
    size_t non_canonical_index;

    // If you have more than 64k files nested, in one translation unit,
    // you have bigger problems.
    uint16_t file_id; 



    union{

        // TT_UNKNOWN, TTPP_UNKNOWN
        struct{
            const char* start;
            size_t len;
        };

        // Number literals
        struct PPNumberLiteral ppNumberLiteral;
    };
} Token;

