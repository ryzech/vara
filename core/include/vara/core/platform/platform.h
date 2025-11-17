#pragma once

#include "vara/core/defines.h"

#include <stdlib.h>

/** */
b8 platform_create(void);

/** */
void platform_destroy(void);

/** */
b8 platform_poll_events(void);

/** */
void* platform_allocate(u64 size);

/** */
void platform_free(void* block);

/** */
void* platform_zero_memory(void* block, u64 size);

/** */
void* platform_copy_memory(void* dst, const void* src, u64 size);

/** */
void* platform_set_memory(void* block, i32 value, u64 size);

/** */
b8 platform_get_random_bytes(void* buf, size_t buf_size);

/** */
void platform_write(const char* message);

/** */
void platform_error(const char* message);

/** */
f64 platform_get_time(void);

/** */
void platform_sleep(u64 milliseconds);
