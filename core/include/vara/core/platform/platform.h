#pragma once

#include <stdlib.h>

#include "vara/core/defines.h"
#include "vara/core/platform/platform_graphics_types.h"

/** */
typedef struct VaraConfig {
    /** Name of the application, may or may not be used depending on the
	 * platform. Some platforms may use it as the window title, others may
	 * ignore it.
	 */
    const char* application_name;
    /** Graphics API (if any) the application will use. */
    PlatformGraphicsType graphics_type;
} VaraConfig;

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
