#pragma once

#include "vara/renderer/swapchain.h"

typedef struct SwapchainBackendVT SwapchainBackendVT;

struct SwapchainBackendVT {
    b8 (*create)(Swapchain* swapchain, const SwapchainConfig* config);
    void (*destroy)(Swapchain* swapchain);
    void (*present)(Swapchain* swapchain);
};