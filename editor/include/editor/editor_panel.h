#pragma once

typedef struct Panel Panel;
typedef struct PanelConfig PanelConfig;

struct Panel {
    const char* id;
    const char* title;
    b8 visible;
    b8 focused;

    void* data;
    size_t data_size;

    b8 (*init)(Panel* self);
    void (*update)(Panel* self, f32 delta);
    void (*draw)(Panel* self);
    void (*destroy)(Panel* self);
};

struct PanelConfig {
    const char* id;
    const char* title;
    size_t data_size;

    b8 (*init)(Panel* self);
    void (*update)(Panel* self, f32 delta);
    void (*draw)(Panel* self);
    void (*destroy)(Panel* self);
};