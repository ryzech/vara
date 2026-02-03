#pragma once

#include <vara/core/defines.h>

typedef struct Swapchain Swapchain;
typedef struct SwapchainConfig SwapchainConfig;

struct VaraWindow;
struct Texture;
struct Renderer;
struct RendererBackend;

struct SwapchainConfig {
    struct VaraWindow* window;
    b8 vsync;
};

struct Swapchain {
    struct VaraWindow* window;
    u32 width, height;
    b8 vsync;
    void* backend_data;
    struct RendererBackend* backend;
};

Swapchain* _swapchain_create(struct RendererBackend* backend, const SwapchainConfig* config);
Swapchain* swapchain_create(struct Renderer* renderer, const SwapchainConfig* config);
void swapchain_destroy(Swapchain* swapchain);
void swapchain_present(Swapchain* swapchain);