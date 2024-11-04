#include <stdio.h>
#include "common/error.h"
#include "common/defs.h"
#include "parser/token.h"


int main(){
    
    struct PPNumberLiteral lit;;


    FalliblePtrResult fpr = parse_number_checked(&lit, "1.1e1f");
    FPR_debug(fpr);
    printf("\nIs Error: %x\n", fpr.is_error);

    ppNumberLiteralDebug(&lit);
    printf("\n");


}