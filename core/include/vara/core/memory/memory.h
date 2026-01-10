#pragma once

#include "vara/core/defines.h"

typedef struct Allocator Allocator;

struct Allocator {
    void* (*alloc)(size_t size, void* context);
    void (*free)(size_t size, void* ptr, void* context);
    void* context;
};

static void* allocator_alloc(Allocator* allocator, size_t size) {
    return allocator->alloc(size, allocator->context);
}

static void allocator_free(Allocator* allocator, size_t size, void* ptr) {
    if (allocator->free) {
        allocator->free(size, ptr, allocator->context);
    }
}

b8 memory_system_create(void);
void memory_system_destroy(void);
size_t memory_system_usage(void);

void* vara_allocate(size_t size);
void vara_free(void* block, size_t size);
void* vara_zero_memory(void* block, size_t size);
void* vara_set_memory(void* block, i32 value, size_t size);
void* vara_copy_memory(void* dst, const void* src, size_t size);