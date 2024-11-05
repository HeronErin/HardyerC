#include "number_parsing.h"
#include "common/defs.h"


// I know it looks like a mess to have 3 levels of wrapping functions for number parsing, but I can explain!
// The method to the madness is that suffixes are a bitch, and therefore need to ALWAYS be checked
// and it makes a bit more sense to do that last.


static FalliblePtrResult _parse_number_checked(struct PPNumberLiteral* literal, char* restrict input);
FalliblePtrResult ppnum_parse_checked(struct PPNumberLiteral* literal, char* restrict input) {
    const FalliblePtrResult res = _parse_number_checked(literal, input);
    if (res.is_error) return res;
    const char trailing_lower = *(char*)res.proper_result | 0x20;

    // Some alpha is trailing, somebody messed up
    if (trailing_lower >= 'a' && trailing_lower <= 'z')
        return FPR_err(res.proper_result, 1, ERR_INVALID_NUMBER_SUFFIX, NULL);
    return res;
}
// Does most of the validation, and type handling
static FalliblePtrResult _parse_number_checked(struct PPNumberLiteral* literal, char* restrict input){
    // Initial number portion (if valid) must be a digit or decimal
    if ( !assess_potential_number(input) )
        return FPR_err(input, 1, ERR_INVALID_NUMBER_PREFIX, NULL);

    const FalliblePtrResult res = ppnum_parse_unchecked(literal, input);
    if (res.is_error) return res;
    char* end = res.proper_result;

    // Now just some more pedantic nitpicks


    // Make sure if the number has a specified base it starts with zero
    if (literal->detected_base != NB_NONE && (literal->predecimal_size != 1 || (literal->postdecimal_size != 0 && *literal->predecimal_portion != '0')))
        return FPR_err((char*) literal->predecimal_portion, literal->predecimal_size + 1, ERR_INVALID_NUMBER_PREFIX, "For bases other than decimal you MUST start with '0' (Ex: 0b101, 0xFEEDBEEF, 022222222)");

    // Base has been set but nothing follows
    if (literal->detected_base != NB_NONE && (literal->postdecimal_size == 0 || literal->postdecimal_portion == NULL))
        return FPR_err((char*) literal->predecimal_portion, 1, ERR_UNEXPECTED_EOL, "Expected something after base specifier");


    // base validation
    switch (literal->detected_base) {
        case NB_BIN:
            for (size_t i = 0; i < literal->postdecimal_size; i++)
                if (literal->postdecimal_portion[i] != '0' && literal->postdecimal_portion[i] != '1')
                    return FPR_err((char*) literal->postdecimal_portion, i+1, ERR_NUM_BASE, "INVALID BINARY: MUST be '1' or '0'");
            break;
        case NB_OCTAL:
            for (size_t i = 0; i < literal->postdecimal_size; i++)
                if ( !(literal->postdecimal_portion[i] >= '0' && literal->postdecimal_portion[i] <= '7') )
                    return FPR_err((char*) literal->postdecimal_portion, i+1, ERR_NUM_BASE, "INVALID OCTAL: MUST be between 1 and 7");
            break;
        default:
    }

    char final_lower = *end | 0x20;
    printf("%c\n", final_lower);

    //  Early return for non alpha ending
    if ( !(final_lower >= 'a' && final_lower <= 'z') ) return res;


    // First suffix may be the float / long suffix
    switch (final_lower) {
        case 'f': literal->detected_type = NBT_FLOAT; return FPR_ok(end+1);
        case 'l': literal->detected_type = NBT_LONG; return FPR_ok(end+1);
        case 'u': literal->detected_type = NBT_UNSIGNED; ++end; break;

        default: return FPR_err(end, 1, ERR_INVALID_NUMBER_SUFFIX, NULL);
    }
    final_lower = *end | 0x20;

    //  Early return for non alpha ending
    if ( !(final_lower >= 'a' && final_lower <= 'z') ) return res;
    // Only possible valid suffix after a 'u' is 'l'  (ACCORDING TO ansi.c.txt)
    if (final_lower == 'l') literal->detected_type |= NBT_LONG;
    else return FPR_err(end, 1, ERR_INVALID_NUMBER_SUFFIX, NULL);

    return FPR_ok(end+1);
}

FalliblePtrResult ppnum_parse_unchecked(struct PPNumberLiteral* literal, char* restrict input){
    char curr, currLower;

    // Init to zero
    literal->postdecimal_size = literal->postE_size = 0;
    // Init to null
    literal->postdecimal_portion = literal->postE_portion = NULL;
    literal->detected_base = literal->detected_type = NB_NONE;


    // Assume that the start of the number is the start of the input
    literal->predecimal_portion = input;

    // Implicit octal
    if (input[0] == '0' && input[1] >= '0' && input[1] <= '9') {
        literal->detected_base = NB_OCTAL;
        literal->predecimal_size = 1;
        goto AFTER_INITIAL_PORTION;
    }


    input--;


    // The initial portion must be a number.
    // Followed by something, non a number
    while (( curr = *(++input) ) && curr >= '0' && curr <= '9');

    const size_t diff = input - literal->predecimal_portion;
    debug_assert(diff <= UINT16_MAX);
    literal->predecimal_size = diff;

    currLower = curr | 0x20;

    if (currLower == 'e') goto E;
    if (currLower == 'x') literal->detected_base = NB_HEX;
    else if (currLower == 'b') literal->detected_base = NB_BIN;
    else if (curr != '.') return FPR_ok(input);

    AFTER_INITIAL_PORTION:


    literal->postdecimal_portion = ++input;

    // At this point some form of separation has occurred (Be is change in base or decimal)

    // Skip normal decimal numbers, and handle E
    if (literal->detected_base == NB_NONE){
        if (*input == '.') return FPR_err(input, 1, ERR_NUM_PERIODS, "Too many periods in number");

        while (( curr = *(++input) ) && curr >= '0' && curr <= '9');
        const size_t postdiff = input - literal->postdecimal_portion;
        debug_assert(postdiff <= UINT16_MAX);
        literal->postdecimal_size = postdiff;

        currLower = curr | 0x20;
        if (currLower == 'e') goto E;
        if (curr == '.') return FPR_err(input, 1, ERR_NUM_PERIODS, "Too many periods in number");
        return FPR_ok(input);
    }else{
        input--;
        // Skip over non-base10 number
        while (// Null check          Decimal number check                Hexadecimal (Case insensitive) check
            ( curr = *(++input) ) && ( (curr >= '0' && curr <= '9')  || (  (curr | 0x20) >= 'a' && (curr | 0x20) <= 'z'  ) )
        );
        const size_t postdiff = input - literal->postdecimal_portion;
        debug_assert(postdiff <= UINT16_MAX);
        literal->postdecimal_size = postdiff;

        return FPR_ok(input);
    }


    // If we made it down here a goto must have fired
    // curr and index* must be equal to e!!!!
    E:
    debug_assert(curr | 0x20 == 'e');
    debug_assert(*input | 0x20 == 'e');
    const char* i2 = input + 1;

    literal->postE_portion = i2;

    if (*i2 == '+' || *i2 == '-') input++;

    while ((curr = *(++input) ) && curr >= '0' && curr <= '9');

    const size_t poste = input - literal->postE_portion;
    debug_assert(poste <= UINT16_MAX);
    literal->postE_size = poste;

    return FPR_ok(input);
}




void ppnum_debug(struct PPNumberLiteral* restrict lit){
    #define COPY_AND_NULL_TERMINATE(BUFFER, size, portion)       \
        char* BUFFER = (size) ? alloca((size) + 1) : "NULL";     \
        if ((size)) {                                            \
            memcpy(BUFFER, (portion), (size));                   \
            BUFFER[(size)] = '\0';                               \
        }
    #define TRUTHY(C) (C) ? "true" : "false"

    COPY_AND_NULL_TERMINATE(predecimal, lit->predecimal_size, lit->predecimal_portion);
    COPY_AND_NULL_TERMINATE(postdecimal, lit->postdecimal_size, lit->postdecimal_portion);
    COPY_AND_NULL_TERMINATE(postE, lit->postE_size, lit->postE_portion);

    printf(
        "<PPNumberLiteral : predecimal: %s, postdecimal: %s, postE: %s, Is float: %s, Is unsigned: %s, Is long: %s>\n",
        predecimal, postdecimal, postE, TRUTHY(lit->detected_type & 1), TRUTHY(lit->detected_type & 4), TRUTHY(lit->detected_type & 2)
    );
}





