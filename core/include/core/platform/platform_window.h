#pragma once

#include "core/defines.h"
#include "core/math/types.h"

/** */
struct VaraWindowState;

/** */
typedef struct VaraWindow {
    /** */
    u32 width;
    /** */
    u32 height;
    /** */
    i32 x;
    /** */
    i32 y;
    /** */
    const char* title;
    /** */
    const char* name;
    /** Pixel density of the given display the window is on. */
    f32 pixel_density;
    /** Window state, holds information about the given window. */
    struct VaraWindowState* platform_state;
} VaraWindow;

/** */
typedef struct VaraWindowConfig {
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
} VaraWindowConfig;

VaraWindow* platform_window_create(const VaraWindowConfig* config);

void platform_window_destroy(VaraWindow* window);

void platform_window_set_title(VaraWindow* window, const char* title);

void platform_window_set_size(VaraWindow* window, u32 width, u32 height);

void platform_window_set_position(VaraWindow* window, i32 x, i32 y);

Vector2i platform_window_get_size(VaraWindow* window);

Vector2i platform_window_get_position(VaraWindow* window);

void* platform_window_get_native_handle(const VaraWindow* window);

void* platform_window_get_proc_address(const char* name);

b8 platform_window_make_context_current(VaraWindow* window);

b8 platform_window_should_close(VaraWindow* window);
