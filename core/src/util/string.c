#include <string.h>

#include "vara/core/platform/platform.h"
#include "vara/core/util/string.h"

u64 string_length(const char* string) {
    if (!string) {
        return 0;
    }
    u32 length = 0;
    for (; length < U32_MAX; length++) {
        if (!string[length]) {
            break;
        }
    }
    return length;
}

char* string_duplicate(const char* string) {
    if (!string) {
        return NULL;
    }
    const u64 length = string_length(string);
    char* copy = platform_allocate(length + 1);
    platform_copy_memory(copy, string, length);
    copy[length] = 0;
    return copy;
}

void string_free(const char* string) {
    if (!string) {
        return;
    }
    platform_free((char*)string);
}

b8 strings_equal(const char* a, const char* b) {
    if (!a || !b) {
        return true;
    }

    // TODO: don't use std string method.
    return strncmp(a, b, U32_MAX) == 0;
}
