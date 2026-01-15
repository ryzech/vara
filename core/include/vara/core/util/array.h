#pragma once

#include "vara/core/defines.h"

typedef struct ArrayHeader ArrayHeader;

struct Allocator;

struct ArrayHeader {
    size_t capacity;
    size_t length;
    size_t stride;
    struct Allocator* allocator;
};

void* _array_create(size_t length, size_t stride, struct Allocator* allocator);
void array_destroy(void* array);

void _array_insert(void* array, size_t index, void* value);
void* _array_append(void* array, void* value);
size_t array_length(void* array);
void array_set_length(void* array, size_t length);

#define array_sized(size, type, allocator) (type*)_array_create(16, sizeof(type), allocator)
#define array(type, allocator)             (type*)_array_create(16, sizeof(type), allocator)

#define array_append(arr, value)                                                                   \
    {                                                                                              \
        typeof(value) _tmp = (value);                                                              \
        (arr) = _array_append((arr), &_tmp);                                                       \
    }