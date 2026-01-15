#include "vara/core/util/array.h"
#include "vara/core/memory/memory.h"

static void* array_resize(void* array) {
    ArrayHeader* header = (ArrayHeader*)array - 1;
    const size_t new_capacity = header->capacity * 2;
    const size_t new_size = new_capacity * header->stride + sizeof(ArrayHeader);
    ArrayHeader* new_header;
    if (header->allocator) {
        new_header = header->allocator->alloc(new_size, header->allocator->context);
    } else {
        new_header = vara_allocate(new_size);
    }
    if (!new_header) {
        return array;
    }
    const size_t old_size = header->capacity * header->stride + sizeof(ArrayHeader);
    vara_copy_memory(new_header, header, old_size);
    if (header->allocator) {
        header->allocator->free(old_size, header, header->allocator->context);
    } else {
        vara_free(header, old_size);
    }
    new_header->capacity = new_capacity;
    return new_header + 1;
}

void* _array_create(size_t length, size_t stride, Allocator* allocator) {
    void* array = NULL;
    const size_t size = length * stride + sizeof(ArrayHeader);
    ArrayHeader* header;
    if (allocator) {
        header = allocator->alloc(size, allocator->context);
    } else {
        header = vara_allocate(size);
    }
    if (header) {
        header->capacity = length;
        header->length = 0;
        header->stride = stride;
        header->allocator = allocator;
        array = header + 1;
    }
    return array;
}

void array_destroy(void* array) {
    if (array) {
        ArrayHeader* header = (ArrayHeader*)array - 1;
        const size_t size = header->capacity * header->stride + sizeof(ArrayHeader);
        if (header->allocator) {
            header->allocator->free(size, header, header->allocator->context);
        } else {
            vara_free(header, size);
        }
    }
}

void _array_insert(void* array, size_t index, void* value) {
}

void* _array_append(void* array, void* value) {
    ArrayHeader* header = (ArrayHeader*)array - 1;
    if (header->length >= header->capacity) {
        array = array_resize(array);
        header = (ArrayHeader*)array - 1;
    }

    vara_copy_memory((u8*)array + header->length * header->stride, value, header->stride);
    header->length++;
    return array;
}

size_t array_length(void* array) {
    const ArrayHeader* header = (ArrayHeader*)array - 1;
    return header->length;
}

void array_set_length(void* array, size_t length) {
    ArrayHeader* header = (ArrayHeader*)array - 1;
    header->length = length;
}
