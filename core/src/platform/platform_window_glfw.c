#include <GLFW/glfw3.h>

#include "vara/core/event/event.h"
#include "vara/core/input/input.h"
#include "vara/core/input/keycodes.h"
#include "vara/core/logger.h"
#include "vara/core/math/types.h"
#include "vara/core/platform/platform_graphics_types.h"
#include "vara/core/platform/platform_window.h"
#include "vara/core/memory/memory.h"

static GLFWcursor* default_cursor;
static GLFWcursor* hand_cursor;
static GLFWcursor* horizontal_cursor;
static GLFWcursor* vertical_cursor;
static GLFWcursor* beam_cursor;
static GLFWcursor* crosshair_cursor;

struct VaraWindowState {
    GLFWwindow* window;
};

static Key glfw_to_keycode(const i32 keycode) {
    switch (keycode) {
        case GLFW_KEY_A:
            return KEY_A;
        case GLFW_KEY_B:
            return KEY_B;
        case GLFW_KEY_C:
            return KEY_C;
        case GLFW_KEY_D:
            return KEY_D;
        case GLFW_KEY_E:
            return KEY_E;
        case GLFW_KEY_F:
            return KEY_F;
        case GLFW_KEY_G:
            return KEY_G;
        case GLFW_KEY_H:
            return KEY_H;
        case GLFW_KEY_I:
            return KEY_I;
        case GLFW_KEY_J:
            return KEY_J;
        case GLFW_KEY_K:
            return KEY_K;
        case GLFW_KEY_L:
            return KEY_L;
        case GLFW_KEY_M:
            return KEY_M;
        case GLFW_KEY_N:
            return KEY_N;
        case GLFW_KEY_O:
            return KEY_O;
        case GLFW_KEY_P:
            return KEY_P;
        case GLFW_KEY_Q:
            return KEY_Q;
        case GLFW_KEY_R:
            return KEY_R;
        case GLFW_KEY_S:
            return KEY_S;
        case GLFW_KEY_T:
            return KEY_T;
        case GLFW_KEY_U:
            return KEY_U;
        case GLFW_KEY_V:
            return KEY_V;
        case GLFW_KEY_W:
            return KEY_W;
        case GLFW_KEY_X:
            return KEY_X;
        case GLFW_KEY_Y:
            return KEY_Y;
        case GLFW_KEY_Z:
            return KEY_Z;
        case GLFW_KEY_0:
            return KEY_0;
        case GLFW_KEY_1:
            return KEY_1;
        case GLFW_KEY_2:
            return KEY_2;
        case GLFW_KEY_3:
            return KEY_3;
        case GLFW_KEY_4:
            return KEY_4;
        case GLFW_KEY_5:
            return KEY_5;
        case GLFW_KEY_6:
            return KEY_6;
        case GLFW_KEY_7:
            return KEY_7;
        case GLFW_KEY_8:
            return KEY_8;
        case GLFW_KEY_9:
            return KEY_9;
        case GLFW_KEY_F1:
            return KEY_F1;
        case GLFW_KEY_F2:
            return KEY_F2;
        case GLFW_KEY_F3:
            return KEY_F3;
        case GLFW_KEY_F4:
            return KEY_F4;
        case GLFW_KEY_F5:
            return KEY_F5;
        case GLFW_KEY_F6:
            return KEY_F6;
        case GLFW_KEY_F7:
            return KEY_F7;
        case GLFW_KEY_F8:
            return KEY_F8;
        case GLFW_KEY_F9:
            return KEY_F9;
        case GLFW_KEY_F10:
            return KEY_F10;
        case GLFW_KEY_F11:
            return KEY_F11;
        case GLFW_KEY_F12:
            return KEY_F12;
        case GLFW_KEY_KP_0:
            return KEY_NUMPAD_0;
        case GLFW_KEY_KP_1:
            return KEY_NUMPAD_1;
        case GLFW_KEY_KP_2:
            return KEY_NUMPAD_2;
        case GLFW_KEY_KP_3:
            return KEY_NUMPAD_3;
        case GLFW_KEY_KP_4:
            return KEY_NUMPAD_4;
        case GLFW_KEY_KP_5:
            return KEY_NUMPAD_5;
        case GLFW_KEY_KP_6:
            return KEY_NUMPAD_6;
        case GLFW_KEY_KP_7:
            return KEY_NUMPAD_7;
        case GLFW_KEY_KP_8:
            return KEY_NUMPAD_8;
        case GLFW_KEY_KP_9:
            return KEY_NUMPAD_9;
        case GLFW_KEY_KP_DECIMAL:
            return KEY_NUMPAD_DECIMAL;
        case GLFW_KEY_KP_DIVIDE:
            return KEY_NUMPAD_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY:
            return KEY_NUMPAD_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT:
            return KEY_NUMPAD_SUBTRACT;
        case GLFW_KEY_KP_ADD:
            return KEY_NUMPAD_ADD;
        case GLFW_KEY_KP_ENTER:
            return KEY_NUMPAD_ENTER;
        case GLFW_KEY_KP_EQUAL:
            return KEY_NUMPAD_EQUAL;
        case GLFW_KEY_ESCAPE:
            return KEY_ESCAPE;
        case GLFW_KEY_ENTER:
            return KEY_ENTER;
        case GLFW_KEY_TAB:
            return KEY_TAB;
        case GLFW_KEY_BACKSPACE:
            return KEY_BACKSPACE;
        case GLFW_KEY_INSERT:
            return KEY_INSERT;
        case GLFW_KEY_DELETE:
            return KEY_DELETE;
        case GLFW_KEY_RIGHT:
            return KEY_RIGHT;
        case GLFW_KEY_LEFT:
            return KEY_LEFT;
        case GLFW_KEY_DOWN:
            return KEY_DOWN;
        case GLFW_KEY_UP:
            return KEY_UP;
        case GLFW_KEY_PAGE_UP:
            return KEY_PAGE_UP;
        case GLFW_KEY_PAGE_DOWN:
            return KEY_PAGE_DOWN;
        case GLFW_KEY_HOME:
            return KEY_HOME;
        case GLFW_KEY_END:
            return KEY_END;
        case GLFW_KEY_CAPS_LOCK:
            return KEY_CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK:
            return KEY_SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK:
            return KEY_NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN:
            return KEY_PRINT_SCREEN;
        case GLFW_KEY_PAUSE:
            return KEY_PAUSE;
        case GLFW_KEY_LEFT_SHIFT:
            return KEY_LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL:
            return KEY_LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT:
            return KEY_LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER:
            return KEY_LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT:
            return KEY_RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL:
            return KEY_RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT:
            return KEY_RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER:
            return KEY_RIGHT_SUPER;
        case GLFW_KEY_APOSTROPHE:
            return KEY_APOSTROPHE;
        case GLFW_KEY_COMMA:
            return KEY_COMMA;
        case GLFW_KEY_MINUS:
            return KEY_MINUS;
        case GLFW_KEY_PERIOD:
            return KEY_PERIOD;
        case GLFW_KEY_SLASH:
            return KEY_SLASH;
        case GLFW_KEY_SEMICOLON:
            return KEY_SEMICOLON;
        case GLFW_KEY_EQUAL:
            return KEY_EQUAL;
        case GLFW_KEY_LEFT_BRACKET:
            return KEY_LBRACKET;
        case GLFW_KEY_BACKSLASH:
            return KEY_BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET:
            return KEY_RBRACKET;
        case GLFW_KEY_GRAVE_ACCENT:
            return KEY_GRAVE;
        case GLFW_KEY_WORLD_1:
            return KEY_WORLD_1;
        case GLFW_KEY_WORLD_2:
            return KEY_WORLD_2;
        case GLFW_KEY_SPACE:
            return KEY_SPACE;
        case GLFW_KEY_MENU:
            return KEY_MENU;

        default:
            return KEY_UNKNOWN;
    }
}

static MouseButton glfw_to_mouse(const i32 keycode) {
    switch (keycode) {
        case GLFW_MOUSE_BUTTON_LEFT:
            return MOUSE_BUTTON_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return MOUSE_BUTTON_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return MOUSE_BUTTON_MIDDLE;
        default:
            return -1;
    }
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const Key keycode = glfw_to_keycode(key);
    const b8 is_pressed = action == GLFW_PRESS || action == GLFW_REPEAT;
    input_system_process_key(keycode, is_pressed);
}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    const MouseButton keycode = glfw_to_mouse(button);
    const b8 is_pressed = action == GLFW_PRESS;
    input_system_process_mouse(keycode, is_pressed);
}

static void glfw_mouse_move_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
    input_system_process_mouse_move((f32)xpos, (f32)ypos);
}

static void glfw_resize_callback(GLFWwindow* window, i32 width, i32 height) {
    VaraWindow* vara_window = glfwGetWindowUserPointer(window);
    vara_window->width = width;
    vara_window->height = height;

    EventData data = {0};
    data.i32[0] = width;
    data.i32[1] = height;
    event_fire(EVENT_WINDOW_RESIZE, NULL, &data);
}

static void glfw_framebuffer_resize_callback(GLFWwindow* window, i32 width, i32 height) {
    VaraWindow* vara_window = glfwGetWindowUserPointer(window);
    vara_window->framebuffer_width = width;
    vara_window->framebuffer_height = height;
}

static void glfw_close_callback(GLFWwindow* window) {
    EventData data = {0};
    // Pass the closed window here somehow?
    event_fire(EVENT_WINDOW_CLOSE, NULL, &data);
}

VaraWindow* platform_window_create(const VaraWindowConfig* config) {
    if (!config) {
        ERROR("platform_window_create: config was NULL");
        return NULL;
    }

    DEBUG("Creating VaraWindow named('%s')", config->name);
    VaraWindow* window = vara_allocate(sizeof(VaraWindow));
    if (!window) {
        ERROR("Failed to allocate VaraWindow");
        return NULL;
    }

    window->platform_state = vara_allocate(sizeof(VaraWindowState));
    if (!window->platform_state) {
        ERROR("Failed to allocate VaraWindowState");
        vara_free(window, sizeof(VaraWindow));
        return NULL;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    if (config->renderer_type == RENDERER_TYPE_OPENGL) {
#ifdef VARA_PLATFORM_APPLE
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#endif
    } else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    GLFWwindow* glfw_window =
        glfwCreateWindow(config->width, config->height, config->title, NULL, NULL);

    if (!glfw_window) {
        ERROR("Failed to create GLFW window.");
        platform_window_destroy(window);
        return NULL;
    }

    window->platform_state->window = glfw_window;
    window->width = config->width;
    window->height = config->height;
    window->x = config->x;
    window->y = config->y;
    window->title = config->title;
    window->name = config->name;
    window->renderer_type = config->renderer_type;

    // Calculate DPI / Pixel Density
    i32 fb_width, fb_height;
    i32 logical_width, logical_height;
    glfwGetFramebufferSize(glfw_window, &fb_width, &fb_height);
    glfwGetWindowSize(glfw_window, &logical_width, &logical_height);

    window->width = logical_width;
    window->height = logical_height;
    window->framebuffer_width = fb_width;
    window->framebuffer_height = fb_height;
    window->pixel_density = (f32)fb_width / (f32)logical_width;

    glfwSetWindowUserPointer(glfw_window, window);

    glfwSetKeyCallback(glfw_window, glfw_key_callback);
    glfwSetMouseButtonCallback(glfw_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(glfw_window, glfw_mouse_move_callback);
    glfwSetWindowSizeCallback(glfw_window, glfw_resize_callback);
    glfwSetFramebufferSizeCallback(glfw_window, glfw_framebuffer_resize_callback);
    glfwSetWindowCloseCallback(glfw_window, glfw_close_callback);

    // TODO: need to update these when switching away from glfw, as GLFW does not support some
    // macos cursors like move resize.
    default_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    hand_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    horizontal_cursor = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    vertical_cursor = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    beam_cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    crosshair_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

    return window;
}

void platform_window_destroy(VaraWindow* window) {
    if (!window || !window->platform_state) {
        return;
    }

    DEBUG("Destroying VaraWindow named('%s')", window->name);
    // Possibly add support for multiple windows later
    if (window->platform_state->window) {
        glfwDestroyWindow(window->platform_state->window);
        window->platform_state->window = NULL;
    }

    vara_free(window->platform_state, sizeof(VaraWindowState));
    window->platform_state = NULL;
    vara_free(window, sizeof(VaraWindow));
}

void platform_window_set_title(VaraWindow* window, const char* title) {
    glfwSetWindowTitle(window->platform_state->window, title);
}

void platform_window_set_cursor(VaraWindow* window, CursorType cursor) {
    GLFWwindow* handle = window->platform_state->window;
    switch (cursor) {
        case CURSOR_NORMAL:
            glfwSetCursor(handle, default_cursor);
            break;
        case CURSOR_HAND:
            glfwSetCursor(handle, hand_cursor);
            break;
        case CURSOR_HORIZONTAL_RESIZE:
            glfwSetCursor(handle, horizontal_cursor);
            break;
        case CURSOR_VERTICAL_RESIZE:
            glfwSetCursor(handle, vertical_cursor);
            break;
        case CURSOR_BEAM:
            glfwSetCursor(handle, beam_cursor);
            break;
        case CURSOR_CROSSHAIR:
            glfwSetCursor(handle, crosshair_cursor);
            break;
    }
}

Vector2i platform_window_get_size(VaraWindow* window) {
    return (Vector2i){
        window->width,
        window->height,
    };
}

Vector2i platform_window_get_framebuffer_size(VaraWindow* window) {
    return (Vector2i){
        window->framebuffer_width,
        window->framebuffer_height,
    };
}

void* platform_window_get_native_handle(const VaraWindow* window) {
    if (!window) {
        return NULL;
    }

    return window->platform_state->window;
}

void* platform_window_get_proc_address(const char* name) {
    if (!name) {
        return NULL;
    }

    return glfwGetProcAddress(name);
}

void platform_window_make_context_current(VaraWindow* window) {
    if (!window || window->renderer_type != RENDERER_TYPE_OPENGL) {
        WARN(
            "Tried to make context current in window. Renderer named('%s') "
            "does not support this. OpenGL only!",
            renderer_type_to_string(window->renderer_type)
        );
        return;
    }

    glfwMakeContextCurrent(window->platform_state->window);
}

void platform_window_set_visible(VaraWindow* window, b8 visible) {
    if (visible) {
        glfwShowWindow(window->platform_state->window);
    } else {
        glfwHideWindow(window->platform_state->window);
    }
}

b8 platform_window_should_close(VaraWindow* window) {
    return glfwWindowShouldClose(window->platform_state->window);
}
