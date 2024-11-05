#include "string_parsing.h"



FalliblePtrResult SL_parse(struct StringLiteral* literal, char* restrict input, const enum StringLiteralVariety variety) {
    debug_assert(variety);

    // Skip past the quotation mark and potential wide indicator
    input += 1 + !!(variety & SV_IS_WIDE);

    literal->variety = variety;
    literal->string_interior = input;

    const char ending_char = input[-1];

    debug_assert(ending_char == '\'' || ending_char == '"');

    bool is_escape = false;
    char curr;
    while (curr = *input) {
        is_escape ^= curr == '\\';
        if (!is_escape && curr == ending_char) break;

        input++;
    }
    size_t len = input - literal->string_interior;
    if (!curr) return FPR_err((char*) literal->string_interior, len, ERR_UNEXPECTED_EOF, "File ended before string was terminated");

    debug_assert(len <= UINT16_MAX);
    literal->string_interior_size = len;

    return FPR_ok(input);
}
