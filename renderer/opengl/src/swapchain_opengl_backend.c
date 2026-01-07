#include <GLFW/glfw3.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/swapchain_opengl_backend.h"

typedef struct OpenGLSwapchainState {
    GLuint fbo;
} OpenGLSwapchainState;

b8 swapchain_opengl_create(Swapchain* swapchain, const SwapchainConfig* config) {
    if (!swapchain || !config) {
        return false;
    }

    DEBUG("Creating Swapchain for VaraWindow named('%s')", config->window->name);
    OpenGLSwapchainState* swapchain_state = platform_allocate(sizeof(OpenGLSwapchainState));
    platform_zero_memory(swapchain_state, sizeof(OpenGLSwapchainState));
    if (!swapchain_state) {
        return false;
    }

    swapchain_state->fbo = 0;
    swapchain->backend_data = swapchain_state;

    return true;
}

void swapchain_opengl_destroy(Swapchain* swapchain) {
    if (!swapchain || !swapchain->backend_data) {
        return;
    }

    OpenGLSwapchainState* swapchain_state = swapchain->backend_data;
    platform_free(swapchain_state);
    swapchain->backend_data = NULL;
}

void swapchain_opengl_present(Swapchain* swapchain) {
    if (!swapchain || !swapchain->backend_data) {
        return;
    }

    glfwSwapInterval(swapchain->vsync);
    glfwSwapBuffers(platform_window_get_native_handle(swapchain->window));
}
