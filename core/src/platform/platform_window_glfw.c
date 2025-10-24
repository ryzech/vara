#include "GLFW/glfw3.h"
#include "core/logger.h"
#include "core/platform/platform.h"
#include "core/platform/platform_window.h"

typedef struct VaraWindowState {
    GLFWwindow* window;
} VaraWindowState;

VaraWindow* platform_window_create(const VaraWindowConfig* config) {
    if (!config) {
        ERROR("platform_window_create: config was NULL");
        return NULL;
    }

    DEBUG("Creating VaraWindow named('%s')", config->name);
    VaraWindow* window = platform_allocate(sizeof(VaraWindow), false);
    if (!window) {
        ERROR("Failed to allocate VaraWindow");
        return NULL;
    }

    window->platform_state =
        platform_allocate(sizeof(VaraWindowState), false);
    if (!window->platform_state) {
        ERROR("Failed to allocate VaraWindowState");
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

    platform_free(window->platform_state, false);
    window->platform_state = NULL;
}

b8 platform_window_should_close(VaraWindow* window) {
    return glfwWindowShouldClose(window->platform_state->window);
}
