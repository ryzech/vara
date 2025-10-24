#pragma once

#include "core/defines.h"

/** */
struct platform_window_state;

/** */
typedef struct platform_window {
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
    struct platform_window_state* platform_state;
} platform_window;

/** */
typedef struct platform_window_config {
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
} platform_window_config;

platform_window* platform_window_create(const platform_window_config* config);

void platform_window_destroy(platform_window* window);

void platform_window_set_title(platform_window* window, const char* title);

void platform_window_set_size(platform_window* window, u32 width, u32 height);

void platform_window_set_position(platform_window* window, i32 x, i32 y);

void platform_window_get_size(platform_window* window, u32* width, u32* height);

void platform_window_get_position(platform_window* window, i32* x, i32* y);

b8 platform_window_should_close(platform_window* window);
