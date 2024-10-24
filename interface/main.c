#include <stdio.h>
#include "parser/patch_resolver.h"
#include "parser/translation.h"


int main(){

    char* x = ":\\\n??=\\\na\\\nf";
    
    PatchString ps = initial_translation_phases(x);
    char* mod = ps_clone_str(&ps);

    printf("%i: \"%s\":\n", ps.internal_string.size, mod);
    
    for (const char* y = mod; *y; y++){
        printf("%c - %c \n",  *y, x[ps_to_original_index(&ps, (size_t)(y-mod))]);
    }
    printf("-----------\n");
    for (const char* y = x; *y; y++){
        printf("%c - %c \n", *y, mod[ps_to_modified_index(&ps, (size_t)(y-x))]);
    }

    ps_free(&ps);
    
}