#pragma once

#include "vara/core/defines.h"

// Should this be size_t? String really shouldn't be bigger than a u64, but in the case it is?
u64 string_length(const char* string);
char* string_duplicate(const char* string);
void string_free(const char* string);