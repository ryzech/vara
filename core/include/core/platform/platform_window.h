#pragma once

#include "core/defines.h"

/** */
struct vara_window_state;

/** */
typedef struct vara_window {
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
    /** Window state, holds information about the given window. */
    struct vara_window_state* platform_state;
} vara_window;

/** */
typedef struct vara_window_config {
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
} vara_window_config;

b8 platform_window_create(vara_window* window, const vara_window_config* config);

void platform_window_destroy(vara_window* window);

b8 platform_window_poll_events(vara_window* window);

void platform_window_set_title(vara_window* window, const char* title);

void platform_window_set_size(vara_window* window, u32 width, u32 height);

void platform_window_set_position(vara_window* window, i32 x, i32 y);

void platform_window_get_size(vara_window* window, u32* width, u32* height);

void platform_window_get_position(vara_window* window, i32* x, i32* y);
