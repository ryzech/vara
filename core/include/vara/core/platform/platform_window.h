#pragma once

#include "vara/core/defines.h"
#include "vara/core/math/types.h"
#include "vara/core/platform/platform_graphics_types.h"

typedef struct VaraWindowState VaraWindowState;
typedef struct VaraWindow VaraWindow;
typedef struct VaraWindowConfig VaraWindowConfig;
typedef enum CursorType CursorType;

enum CursorType {
    CURSOR_NORMAL,
    CURSOR_HAND,
    CURSOR_HORIZONTAL_RESIZE,
    CURSOR_VERTICAL_RESIZE,
    CURSOR_BEAM,
    CURSOR_CROSSHAIR
};

/** */
struct VaraWindow {
    /** Window state, holds information about the given window. */
    VaraWindowState* platform_state;
    /** */
    const char* title;
    /** */
    const char* name;
    /** */
    i32 width, height;
    /** */
    i32 framebuffer_width, framebuffer_height;
    /** */
    i32 x, y;
    /** Pixel density of the given display the window is on. */
    f32 pixel_density;
    /** Type of graphics API to use (decides whether to create a window context). */
    PlatformRendererType renderer_type;
};

/** */
struct VaraWindowConfig {
    /** */
    const char* title;
    /** */
    const char* name;
    /** */
    i32 width, height;
    /** */
    i32 x, y;
    /** Type of graphics API to use (decides whether or not to create a window context). */
    PlatformRendererType renderer_type;
};

VaraWindow* platform_window_create(const VaraWindowConfig* config);
void platform_window_destroy(VaraWindow* window);

void platform_window_set_title(VaraWindow* window, const char* title);
void platform_window_set_size(VaraWindow* window, u32 width, u32 height);
void platform_window_set_position(VaraWindow* window, i32 x, i32 y);
void platform_window_set_cursor(VaraWindow* window, CursorType cursor);
Vector2i platform_window_get_size(VaraWindow* window);
Vector2i platform_window_get_framebuffer_size(VaraWindow* window);
Vector2i platform_window_get_position(VaraWindow* window);
void* platform_window_get_native_handle(const VaraWindow* window);
void* platform_window_get_proc_address(const char* name);
void platform_window_make_context_current(VaraWindow* window);
void platform_window_set_visible(VaraWindow* window, b8 visible);
b8 platform_window_should_close(VaraWindow* window);
