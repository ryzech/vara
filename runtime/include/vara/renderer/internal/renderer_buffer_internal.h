#pragma once

#include "vara/renderer/buffer.h"

typedef struct BufferBackendVT BufferBackendVT;

struct BufferBackendVT {
    b8 (*create)(Buffer* buffer, const BufferConfig* config);
    void (*destroy)(Buffer* buffer);
    void (*bind)(Buffer* buffer);
    void (*unbind)(Buffer* buffer);
    void (*set_data)(Buffer* buffer, const void* data, size_t size, size_t offset);
};