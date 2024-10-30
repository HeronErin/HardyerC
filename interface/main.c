#include <stdio.h>
#include "common/error.h"
#include "common/defs.h"
#include "parser/token.h"


int main(){
    
    struct PPNumberLiteral lit;;


    FalliblePtrResult fpr = parse_number(&lit, "0xDEADBEEF");
    FPR_debug(fpr);
    printf("\nIs Error: %x\n", fpr.is_error);

    ppNumberLiteralDebug(&lit);
    printf("\n");


}