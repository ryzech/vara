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
    Application app;
    const char* name;
    VaraWindowConfig* window_config;
    LogLevel level;
    PlatformRendererType renderer_type;
};

int application_main(int argc, char** argv);

VaraWindow* application_get_window(void);
void application_exit(void);

void application_init(ApplicationConfig* config);
