#pragma once

#include "core/defines.h"

/** */
struct window_platform_state;

/** */
typedef struct window {
    /** */
    u32 width;
    /** */
    u32 height;
    /** */
    const char* title;
    /** */
    const char* name;
    /** Pixel density of the given display the window is on. */
    f32 pixel_density;
    /** Platform state for the window. */
    struct window_platform_state* platform_state;
} window;

/** */
typedef struct window_config {
    /** */
    i32 position_x;
    /** */
    i32 position_y;
    /** */
    u32 width;
    /** */
    u32 height;
    /** */
    const char* title;
    /** */
    const char* name;
} window_config;

/** */
struct platform_state;

/** */
typedef struct platform_config {
    /** Name of the application, may or may not be used depending on the
	 * platform. Some platforms may use it as the window title, others may
	 * ignore it.
	 */
    const char* application_name;
} platform_config;

/** */
b8 platform_init(struct platform_state* state, const char* application_name, i32 x, i32 y, i32 width, i32 height);
/** */
void platform_destroy(struct platform_state* state);

/** */
b8 platform_poll_event(struct platform_state* state);

/** */
void* platform_allocate(u64 size, b8 aligned);

/** */
void platform_free(void* block, b8 aligned);

/** */
void* platform_zero_memory(void* block, u64 size);

/** */
void* platform_copy_memory(void* dst, const void* src, u64 size);

/** */
void* platform_set_memory(void* block, i32 value, u64 size);

/** */
void platform_write(const char* message);

/** */
void platform_error(const char* message);

/** */
f64 platform_get_time(void);

/** */
void platform_sleep(u64 milliseconds);
