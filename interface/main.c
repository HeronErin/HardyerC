#include <stdio.h>
#include "parser/patch_resolver.h"

int main(){
    PatchString ps = ps_from("Hello");
    printf("%s\n", ps_clone_str(&ps));
    ps_insert_at_end(&ps, " World!");
    printf("%s\n", ps_clone_str(&ps));


    ps_insert_at(&ps, 20000, "");


    ps_free(&ps);
    
}