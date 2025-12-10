#pragma once

#include <flecs.h>

typedef struct Scene Scene;

struct Scene {
    ecs_world_t* world;
};

Scene scene_create(void);
void scene_destroy(Scene scene);