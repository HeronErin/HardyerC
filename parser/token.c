#include "token.h"
#include "patch_resolver.h"
#include "state.h"
#include "common/defs.h"
#include "common/array.h"
#include "common/error.h"
#include <stdbool.h>


// // Returns array of tokens. 
// Array pp_tokenize(TranslationParserState* state, const char* str, uint16_t file_id){
//     const char* origin = str;
//     Array tokens = array_new_with_capacity(Token, 2048);

//     while (*str){
//         if (assess_potential_number(str)){
//             Token* number = array_push_unallocated(&tokens, sizeof(Token));
//             number->type = TTPP_PP_NUMBER;
//             number->non_canonical_index = str - origin;
//             number->file_id = file_id;

//             str = parse_number(&number->ppNumberLiteral, str);
//             continue;
//         }else{
            
//         }

//     }



    
//     return tokens;
// }


FalliblePtrResult parse_number_checked(struct PPNumberLiteral* literal, char* restrict input){
    // Initial number portion (if valid) must be a digit or decimal
    if ( !assess_potential_number(input) ) 
        return FPR_err(input, 1, ERR_INVALID_NUMBER_PREFIX, NULL);

    FalliblePtrResult res = parse_number_unchecked(literal, input);
    if (res.is_error) return res;


    // Now just some more pedantic nitpicks


    // Make sure if the number has a specified base it starts with zero
    if (literal->detected_base != NB_NONE && (literal->predecimal_size != 1 || (literal->postdecimal_size != 0 && *literal->predecimal_portion != '0')))
        return FPR_err((char*) literal->predecimal_portion, literal->predecimal_size + 1, ERR_INVALID_NUMBER_PREFIX, "For bases other than decimal you MUST start with '0' (Ex: 0b101, 0xFEEDBEEF, 022222222)");

    // Base has been set but nothing follows
    if (literal->detected_base != NB_NONE && (literal->postdecimal_size == 0 || literal->postdecimal_portion == NULL))
        return FPR_err((char*) literal->predecimal_portion, 1, ERR_UNEXPECTED_EOL, "Expected something after base specifier");

    // Binary base validation
    if (literal->detected_base == NB_BIN)
        for (size_t i = 0; i < literal->postdecimal_size; i++)
            if (literal->postdecimal_portion[i] != '0' && literal->postdecimal_portion[i] != '1')
                return FPR_err((char*) literal->predecimal_portion, i+1, ERR_NUM_BASE, "INVALID BINARY: MUST be '1' or '0'");

    if (literal->detected_base == NB_OCTAL)
        for (size_t i = 0; i < literal->postdecimal_size; i++)
            if ( !(literal->postdecimal_portion[i] >= '0' && literal->postdecimal_portion[i] <= '7') )
                return FPR_err((char*) literal->predecimal_portion, i+1, ERR_NUM_BASE, "INVALID OCTAL: MUST be between 1 and 7");

    return res;

}

FalliblePtrResult parse_number_unchecked(struct PPNumberLiteral* literal, char* restrict input){
    char curr, currLower;

    // Init to zero
    literal->postdecimal_size = literal->postE_size = 0;
    // Init to null
    literal->postdecimal_portion = literal->postE_portion = NULL;
    literal->detected_base = NB_NONE;


    // Assume that the start of the number is the start of the input
    literal->predecimal_portion = input;


    
    // Implicit octal
    if (input[0] == '0' && input[1] >= '0' && input[1] <= '9') {
        literal->detected_base = NB_OCTAL;
        literal->predecimal_size = 1;
        goto AFTER_INITIAL_PORTION;
    }


    input--;


    // The initial portion must be a number. 
    // Followed by something, non a number
    while (( curr = *(++input) ) && curr >= '0' && curr <= '9');



    literal->predecimal_size = input - literal->predecimal_portion;

    currLower = curr | 0x20;
    
    if (currLower == 'e') goto E;
    if (currLower == 'x') literal->detected_base = NB_HEX;
    else if (currLower == 'b') literal->detected_base = NB_BIN;
    else if (curr != '.') return FPR_ok(input);

    AFTER_INITIAL_PORTION:


    literal->postdecimal_portion = ++input;

    // At this point some form of separation has occurred (Be is change in base or decimal)

    // Skip normal decimal numbers, and handle E
    if (literal->detected_base == NB_NONE){
        while (( curr = *(++input) ) && curr >= '0' && curr <= '9');
        literal->postdecimal_size = input - literal->postdecimal_portion;

        currLower = curr | 0x20;
        if (currLower == 'e') goto E;
        return FPR_ok(input-1);
    }else{
        input--;
        // Skip over non-base10 number
        while (// Null check          Decimal number check                Hexadecimal (Case insensitive) check
            ( curr = *(++input) ) && ( (curr >= '0' && curr <= '9')  || (  (curr | 0x20) >= 'a' && (curr | 0x20) <= 'f'  ) )
        );
        literal->postdecimal_size = input - literal->postdecimal_portion;
        return FPR_ok(input-1);
    }


    // If we made it down here a goto must have fired
    // curr and index* must be equal to e!!!!
    E:
    debug_assert(curr | 0x20 == 'e');
    debug_assert(*input | 0x20 == 'e');
    const char* i2 = input + 1;

    literal->postE_portion = i2;
    
    if (*i2 == '+' || *i2 == '-') input++;

    while ((curr = *(++input) ) && curr >= '0' && curr <= '9');

    literal->postE_size = input - literal->postE_portion;

    return FPR_ok(input);
}




void ppNumberLiteralDebug(struct PPNumberLiteral* restrict lit){
    #define COPY_AND_NULL_TERMINATE(BUFFER, size, portion)       \
        char* BUFFER = (size) ? alloca((size) + 1) : "NULL";     \
        if ((size)) {                                            \
            memcpy(BUFFER, (portion), (size));                   \
            BUFFER[(size)] = '\0';                               \
        }


    COPY_AND_NULL_TERMINATE(predecimal, lit->predecimal_size, lit->predecimal_portion);
    COPY_AND_NULL_TERMINATE(postdecimal, lit->postdecimal_size, lit->postdecimal_portion);
    COPY_AND_NULL_TERMINATE(postE, lit->postE_size, lit->postE_portion);

    printf(
        "<PPNumberLiteral : predecimal: %s, postdecimal: %s, postE: %s>", predecimal, postdecimal, postE
    );
}





