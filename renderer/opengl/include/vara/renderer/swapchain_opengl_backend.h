#pragma once

#include <vara/core/defines.h>
#include <vara/renderer/swapchain.h>

b8 swapchain_opengl_create(Swapchain* swapchain, const SwapchainConfig* config);
void swapchain_opengl_destroy(Swapchain* swapchain);
void swapchain_opengl_present(Swapchain* swapchain);