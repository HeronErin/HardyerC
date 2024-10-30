#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "common/error.h"

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
    }
    return false;
}


static inline bool assess_potential_number(const char* restrict input){
    return (input[0] >= '0' && input[0] <= '9') || (input[0] == '.' && input[1] >= '0' && input[1] <= '9');
}

enum NumberBase{
    NB_NONE,
    NB_OCTAL,
    NB_HEX,
    NB_BIN
};

struct PPNumberLiteral{
    enum NumberBase detected_base;


    // NO MORE THAN 64k long numbers
    // Also for the sake of parsing the x in 0x12 is called the decimal
    uint16_t predecimal_size;
    uint16_t postdecimal_size;
    uint16_t postE_size;

    const char* predecimal_portion;
    const char* postdecimal_portion;
    const char* postE_portion;
};

void ppNumberLiteralDebug(struct PPNumberLiteral* restrict lit);




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



FalliblePtrResult parse_number(struct PPNumberLiteral* literal, char* restrict input);