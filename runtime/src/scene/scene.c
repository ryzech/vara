#include "vara/scene/scene.h"
#include "vara/scene/component.h"

Scene scene_create(void) {
    ecs_world_t* scene_world = ecs_init();

    // Likely should register elsewhere, or have a function that does this.
    ECS_COMPONENT_DEFINE(scene_world, TransformComponent);

    return (Scene){
        .world = scene_world,
    };
}

void scene_destroy(Scene scene) {
    if (scene.world != NULL) {
        ecs_fini(scene.world);
    }
}
