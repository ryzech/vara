#pragma once

#include "vara/core/memory/memory.h"

typedef struct Arena Arena;

struct Arena {
    void* buffer;
    size_t size;
    size_t offset;
    size_t used;
};

Arena arena_create(size_t size);
Allocator arena_allocator(Arena* arena);
void arena_reset(Arena* arena);
void arena_destroy(Arena* arena);