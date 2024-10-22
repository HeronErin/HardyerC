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
    
    for (const char* y = x; *y; ++y){
        printf("%c - %c \n",  mod[ps_to_modified_index(&ps, (size_t)(y-x))], *y);
    }


    // ps_free(&ps);
    
}