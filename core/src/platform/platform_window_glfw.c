#include "GLFW/glfw3.h"

#include "vara/core/logger.h"
#include "vara/core/math/types.h"
#include "vara/core/platform/platform.h"
#include "vara/core/platform/platform_window.h"

typedef struct VaraWindowState {
    GLFWwindow* window;
} VaraWindowState;

VaraWindow* platform_window_create(const VaraWindowConfig* config) {
    if (!config) {
        ERROR("platform_window_create: config was NULL");
        return NULL;
    }

    DEBUG("Creating VaraWindow named('%s')", config->name);
    VaraWindow* window = platform_allocate(sizeof(VaraWindow));
    DEBUG("Allocated window.");
    if (!window) {
        ERROR("Failed to allocate VaraWindow");
        return NULL;
    }

    DEBUG("Allocating window state");
    window->platform_state = platform_allocate(sizeof(VaraWindowState));
    DEBUG("Allocated window state");
    if (!window->platform_state) {
        ERROR("Failed to allocate VaraWindowState");
        platform_free(window);
        return NULL;
    }

    glfwDefaultWindowHints();

#ifdef VARA_PLATFORM_APPLE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    DEBUG("Creating glfw window");
    GLFWwindow* glfw_window = glfwCreateWindow(
        config->width, config->height, config->title, NULL, NULL
    );
    DEBUG("Created glfw window.");

    if (!glfw_window) {
        ERROR("Failed to create GLFW window.");
        platform_free(window->platform_state);
        platform_free(window);
        return NULL;
    }

    window->platform_state->window = glfw_window;
    window->width = config->width;
    window->height = config->height;
    window->x = config->position_x;
    window->y = config->position_y;
    window->title = config->title;
    window->name = config->name;
    window->pixel_density = 1.0f;

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

    platform_free(window->platform_state);
    window->platform_state = NULL;
}

void platform_window_set_title(VaraWindow* window, const char* title) {
    glfwSetWindowTitle(window->platform_state->window, title);
}

Vector2i platform_window_get_size(VaraWindow* window) {
    i32 width, height;
    glfwGetWindowSize(window->platform_state->window, &width, &height);
    Vector2i dimensions = {.x = width, .y = height};

    return dimensions;
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

b8 platform_window_make_context_current(VaraWindow* window) {
    if (!window) {
        return false;
    }

    glfwMakeContextCurrent(window->platform_state->window);
    return true;
}

b8 platform_window_should_close(VaraWindow* window) {
    return glfwWindowShouldClose(window->platform_state->window);
}
