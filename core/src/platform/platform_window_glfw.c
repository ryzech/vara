#include "GLFW/glfw3.h"
#include "core/logger.h"
#include "core/platform/platform.h"
#include "core/platform/platform_window.h"

typedef struct platform_window_state {
    GLFWwindow* window;
} platform_window_state;

platform_window* platform_window_create(const platform_window_config* config) {
    if (!config) {
        ERROR("platform_window_create: config was NULL");
        return NULL;
    }

    platform_window* window = platform_allocate(sizeof(platform_window), false);
    if (!window) {
        ERROR("Failed to allocate platform_window");
        return NULL;
    }

    window->platform_state =
        platform_allocate(sizeof(platform_window_state), false);
    if (!window->platform_state) {
        ERROR("Failed to allocate platform_window_state");
        platform_free(window, false);
        return NULL;
    }

    GLFWwindow* glfw_window = glfwCreateWindow(
        config->width, config->height, config->title, NULL, NULL
    );

    if (!glfw_window) {
        ERROR("Failed to create GLFW window.");
        platform_free(window->platform_state, false);
        platform_free(window, false);
        return NULL;
    }

    window->platform_state->window = glfw_window;
    window->width = config->width;
    window->height = config->height;
    window->title = config->title;
    window->name = config->name;
    window->pixel_density = 1.0f;

    return window;
}

void platform_window_destroy(platform_window* window) {
    if (!window || !window->platform_state) {
        return;
    }

    // Possibly add support for multiple windows later
    if (window->platform_state->window) {
        glfwDestroyWindow(window->platform_state->window);
        window->platform_state->window = NULL;
    }

    platform_free(window->platform_state, false);
    window->platform_state = NULL;
}

b8 platform_window_should_close(platform_window* window) {
    return glfwWindowShouldClose(window->platform_state->window);
}
