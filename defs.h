#pragma once

#ifdef DEBUG

#include <assert.h>
#define debug_assert(EXPR) assert(EXPR)

#else
#define debug_assert(EXPR) {}

#endif