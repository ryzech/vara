#pragma once

#include <vara/core/defines.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform_window.h>

#include "vara/renderer/renderer.h"

typedef struct Application Application;
typedef struct ApplicationConfig ApplicationConfig;

struct Application {
    void (*on_init)(void);
    void (*on_update)(f32 delta_time);
    void (*on_shutdown)(void);
    void (*on_resize)(void);
};

struct ApplicationConfig {
    const char* name;
    LogLevel level;
    VaraWindowConfig* window_config;
    PlatformGraphicsType graphics_type;
    Application app;
};

int application_main(int argc, char** argv);

VaraWindow* application_get_window(void);
RendererInstance* application_get_renderer(void);
void application_exit(void);

void application_init(ApplicationConfig* config);
