#pragma once
#include "common/error.h"


static inline bool assess_potential_number(const char* restrict input){
    return (input[0] >= '0' && input[0] <= '9') || (input[0] == '.' && input[1] >= '0' && input[1] <= '9');
}

__attribute__ ((__packed__)) enum NumberBase{
    NB_NONE,
    NB_OCTAL,
    NB_HEX,
    NB_BIN
};

__attribute__ ((__packed__)) enum NumberTypeSuffix {
    NBT_NONE = 0, // Bit 0: is float
    NBT_FLOAT = 1,
    NBT_LONG = 2, // Bit 1: is long
    NBT_UNSIGNED = 4 // Bit 2: is unsigned
};



struct PPNumberLiteral{
    enum NumberBase detected_base;
    enum NumberTypeSuffix detected_type;


    // NO MORE THAN 64k long numbers
    // Also for the sake of parsing the x in 0x12 is called the decimal
    uint16_t predecimal_size;
    uint16_t postdecimal_size;
    uint16_t postE_size;

    const char* predecimal_portion;
    const char* postdecimal_portion;
    const char* postE_portion;
};

void ppNumberLiteralDebug(struct PPNumberLiteral* restrict lit);

// Parses number, and returns an error if invalid
FalliblePtrResult parse_number_checked(struct PPNumberLiteral* literal, char* restrict input);

// Mostly unchecked number parsing, also ignores type suffixes
FalliblePtrResult parse_number_unchecked(struct PPNumberLiteral* literal, char* restrict input);



