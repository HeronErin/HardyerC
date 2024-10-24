#include <stdio.h>
#include "parser/patch_resolver.h"


ConstructionResult on_window(const char* window){
    

    if (window == NULL) return CR_NOP;

    if (window[0] == 'F'){
        return (ConstructionResult) {
            CR_DISCARD_AND_INSERT,
            "B",
            1
        };
    }
    if (window[0] == 'B'){
        return (ConstructionResult) {
            CR_DISCARD_AND_INSERT,
            "F",
            1
        };
    }

    return CR_NOP;
}

int main(){
    const char* x = "Boo Far Faz!";
    PatchString ps = ps_construct_from(x, 3, &on_window);
    
    const char* mod = ps_clone_str(&ps);
    printf("\"%s\":\n", mod);
    
    for (const char* y = mod; *y; y++){
        printf("%c - %c \n",  *y, x[ps_to_original_index(&ps, (size_t)(y-mod))]);
    }
    printf("-----------\n");
    for (const char* y = x; *y; y++){
        printf("%c - %c \n", *y, mod[ps_to_modified_index(&ps, (size_t)(y-x))]);
    }

    // ps_free(&ps);
    
}