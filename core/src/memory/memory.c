#include "vara/core/memory/memory.h"
#include "vara/core/platform/platform.h"

typedef struct MemoryState {
    size_t total_memory_usage;
} MemoryState;

static MemoryState* memory_state;

b8 memory_system_create(void) {
    memory_state = platform_allocate(sizeof(MemoryState));
    platform_zero_memory(memory_state, sizeof(MemoryState));
    if (!memory_state) {
        return false;
    }

    return true;
}

void memory_system_destroy(void) {
    if (memory_state) {
        platform_free(memory_state);
    }
}

size_t memory_system_usage(void) {
    if (memory_state) {
        return memory_state->total_memory_usage;
    }

    return 0;
}

void* vara_allocate(size_t size) {
    void* block = platform_allocate(size);
    if (block) {
        memory_state->total_memory_usage += size;
        return block;
    }

    return NULL;
}

void vara_free(void* block, size_t size) {
    if (!block || !size) {
        return;
    }

    platform_free(block);
    memory_state->total_memory_usage -= size;
}

void* vara_zero_memory(void* block, size_t size) {
    if (block && size) {
        return platform_zero_memory(block, size);
    }

    return NULL;
}

void* vara_set_memory(void* block, i32 value, size_t size) {
    if (block && size) {
        return platform_set_memory(block, value, size);
    }

    return NULL;
}

void* vara_copy_memory(void* dst, const void* src, size_t size) {
    if (dst && src && size) {
        return platform_copy_memory(dst, src, size);
    }

    return NULL;
}
