#pragma once

#include <flecs.h>
#include <vara/core/defines.h>

typedef struct Scene Scene;

struct RenderContext;

struct Scene {
    ecs_world_t* world;
    struct RenderContext* render_context;
};

Scene scene_create(struct RenderContext* render_context);
void scene_destroy(Scene scene);

void scene_update(Scene scene, f32 delta);