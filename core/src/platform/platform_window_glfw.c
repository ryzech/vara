#include "GLFW/glfw3.h"
#include "core/logger.h"
#include "core/platform/platform.h"
#include "core/platform/platform_window.h"

typedef struct vara_window_state {
    GLFWwindow* window;
} vara_window_state;

b8 platform_window_create(
    vara_window* window, const vara_window_config* config
) {
    if (!window || !config) {
        return false;
    }

    window->platform_state =
        (vara_window_state*)platform_allocate(sizeof(vara_window_state), false);
    if (!window->platform_state) {
        return false;
    }

    if (glfwInit() != true) {
        ERROR("Failed to initalize GLFW window platform.");
    }

    window->width = config->width;
    window->height = config->height;
    window->title = config->title;
    window->name = config->name;
    window->pixel_density = 1.0f;

    window->platform_state->window = glfwCreateWindow(
        config->width, config->height, config->title, NULL, NULL
    );

    if (!window->platform_state->window) {
        platform_free(window->platform_state, false);
        return false;
    }

    return true;
}

void platform_window_destroy(vara_window* window) {
    if (!window || !window->platform_state) {
        return;
    }

    if (window->platform_state->window) {
        glfwDestroyWindow(window->platform_state->window);
        window->platform_state->window = NULL;
        glfwTerminate();
    }

    platform_free(window->platform_state, false);
    window->platform_state = NULL;
}

b8 platform_window_poll_events(void) {
    glfwPollEvents();
    return true;
}
