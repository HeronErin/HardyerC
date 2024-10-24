#include "translation.h"

// Must have a window size of 3!
// Replaces trigraphs into their corresponding meanings
ConstructionResult _trigraph_predicate(const char* restrict window){
    // Has no state, so no need for setup
    if (window == NULL)
        return CR_NOP;

    if (window[0] != '?' || window[1] != '?') return (ConstructionResult){ CR_KEEP };

    char replacement = 0;

    switch (window[2]){
        #define _TRI(X, Y) case (X): replacement = (Y); break
         _TRI('=',  '#');
         _TRI('(',  '[');
         _TRI('/',  '\\');
         _TRI(')',  ']');
         _TRI('\'', '^');
         _TRI('<',  '{');
         _TRI('!',  '|');
         _TRI('>',  '}');
         _TRI('-',  '~');

        default: return (ConstructionResult){ CR_KEEP };
    };
    return (ConstructionResult){
        CR_DISCARD_AND_INSERT_CHAR,
        NULL,
        3,
        replacement
    };

    
}

PatchString compute_trigraphs(const char* input){
    return ps_construct_from(input, 3, &_trigraph_predicate);
}


// Must have a window size of 2!
// "\\\n" -> ""
static ConstructionResult _logical_newlines_predicate(const char* restrict window){
    static bool was_last_char_backslash = false;

    // Set up state
    if (window == NULL){
        was_last_char_backslash = false;
        return CR_NOP;
    }
    if (!was_last_char_backslash && window[0] == '\\' && window[1] == '\n'){
        was_last_char_backslash = false;
        return (ConstructionResult){
            CR_DISCARD,
            NULL,
            2
        };
    }
    was_last_char_backslash ^= (window[0] == '\\');
    return (ConstructionResult){CR_KEEP};

}





PatchString initial_translation_phases(const char* input){
    PatchString ret = ps_construct_from(input, 3, &_trigraph_predicate);
    ps_reconstruct_using(&ret, 2, &_logical_newlines_predicate);

    return ret;    
}

