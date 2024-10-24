#include "translation.h"

// Must have a window size of 3!
static ConstructionResult _trigraph_predicate(const char* restrict window){
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


