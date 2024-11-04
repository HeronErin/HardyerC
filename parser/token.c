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
