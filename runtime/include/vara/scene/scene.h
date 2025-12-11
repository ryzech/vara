#pragma once

#include <flecs.h>
#include <vara/core/defines.h>

typedef struct Scene Scene;

struct Scene {
    ecs_world_t* world;
};

Scene scene_create(void);
void scene_destroy(Scene scene);

void scene_update(Scene scene, f32 delta);