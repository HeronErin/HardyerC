#include <stdio.h>
#include "common/error.h"
#include "parser/string_parsing.h"



int main(){
    
    struct StringLiteral lit;

    char* s = "l\"hello world\"q";

    enum StringLiteralVariety slv = SL_assess(s);
    printf("%x\n", slv);
    FalliblePtrResult fpr =  SL_parse(&lit, s, slv);
    printf("%c\n", fpr.is_error + '0');
    FPR_debug(fpr);
    printf("%s\n", lit.string_interior);
    SL_debug(&lit);



}