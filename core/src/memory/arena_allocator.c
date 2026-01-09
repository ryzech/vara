#include "vara/core/memory/arena_allocator.h"
#include "vara/core/platform/platform.h"

static size_t align_forward(size_t ptr, size_t alignment) {
    const size_t modulo = ptr & (alignment - 1);
    if (modulo != 0) {
        ptr += alignment - modulo;
    }
    return ptr;
}

static void* arena_alloc_aligned(Arena* arena, size_t size, size_t alignment) {
    const size_t offset = align_forward(arena->offset, alignment);
    if (offset + size > arena->size) {
        return NULL;
    }

    arena->used += size;
    void* ptr = (u8*)arena->buffer + offset;
    arena->offset = offset + size;

    return ptr;
}

static void* arena_alloc(size_t size, void* context) {
    if (!size) {
        return NULL;
    }

    return arena_alloc_aligned((Arena*)context, size, 2 * sizeof(void*));
}

static void arena_free(size_t size, void* ptr, void* context) {
    (void)size;
    (void)ptr;
    (void)context;
}

Arena arena_create(size_t size) {
    if (!size) {
        size = MiB(16);
    }

    void* buffer = platform_allocate(size);
    return (Arena){
        .buffer = buffer,
        .size = size,
    };
}

Allocator arena_allocator(Arena* arena) {
    return (Allocator){
        .alloc = arena_alloc,
        .free = arena_free,
        .context = arena,
    };
}

void arena_reset(Arena* arena) {
    arena->offset = 0;
    arena->used = 0;
}

void arena_destroy(Arena* arena) {
    if (arena->buffer) {
        platform_free(arena->buffer);
    }
}
