#include <stdio.h>
#include "parser/patch_resolver.h"


ConstructionResult on_window(const char* window){
    

    if (window == NULL) return CR_NOP;
    if (window[0] == ' '){
        return (ConstructionResult) {
            CR_DISCARD,
            0, 0,
            1
        };
    }

    return CR_NOP;
}

int main(){
    const char* x = "Foo Bar Baz";
    PatchString ps = _ps_windowed_construction(x, strlen(x), 3, &on_window);

    const char* mod = ps_clone_str(&ps);
    
    for (const char* y = mod; *y; ++y){
        printf("%c - %c \n",  *y, x[ps_to_original_index(&ps, (size_t)(y-mod))]);
    }


    // ps_free(&ps);
    
}