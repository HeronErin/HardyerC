#pragma once
#include <stdint.h>
#include <common/error.h>

enum StringLiteralVariety {
    SV_IS_VALID = 1,

    SV_IS_STRING = 0b10,

    SV_IS_WIDE = 0b100,  // Bit 2: Is wide


} __attribute((__packed__));

struct StringLiteral {
    enum StringLiteralVariety variety;

    uint16_t string_interior_size;
    const char* string_interior;

};

static inline enum StringLiteralVariety SL_assess(const char* restrict input){
    if (input[0] == '\'') return SV_IS_VALID;
    if (input[0] == '"') return SV_IS_STRING | SV_IS_VALID;
    if ((input[0] | 0x20) != 'l') return 0;
    if (input[1] == '\'') return  SV_IS_VALID | SV_IS_WIDE;
    if (input[1] == '"') return SV_IS_STRING | SV_IS_VALID | SV_IS_WIDE;

    return 0;
}

FalliblePtrResult SL_parse(struct StringLiteral* literal, char* restrict input, const enum StringLiteralVariety variety);


static inline void SL_debug(struct StringLiteral* literal) {
    #define COPY_AND_NULL_TERMINATE(BUFFER, size, portion)       \
        char* BUFFER = (size) ? alloca((size) + 1) : "NULL";     \
        if ((size)) {                                            \
            memcpy(BUFFER, (portion), (size));                   \
            BUFFER[(size)] = '\0';                               \
        }

    COPY_AND_NULL_TERMINATE(interior, literal->string_interior_size, literal->string_interior);

    char wc = literal->variety & SV_IS_WIDE ? 'L' : ' ';
    char wrap = literal->variety & SV_IS_STRING ? '\"' : '\'';
    printf("<StringLiteral : %c%c%s%c>\n", wc, wrap, interior, wrap );
}
