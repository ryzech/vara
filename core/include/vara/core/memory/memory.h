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