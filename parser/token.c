#include "token.h"
#include "patch_resolver.h"
#include "state.h"
#include "common/defs.h"
#include "common/array.h"
#include <stdbool.h>


// Returns array of tokens. 
Array pp_tokenize(TranslationParserState* state, const char* restrict str, uint16_t file_id){
    Array tokens = array_new_with_capacity(Token, 2048);





    
    return tokens;
}

// Returns: true if successful
// Initializes the number based on input
// Requires the input param to be null terminated
// DOES NOT VALIDATE the number, assumes 
bool parse_number_pre(struct PPNumberLiteral* literal, const char* restrict input){
    input--;
    while (*(++input)){
        
    }
}










