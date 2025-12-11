#include <vara/core/math/math.h>

#include "vara/renderer/texture.h"
#include "vara/renderer2d/renderer2d.h"
#include "vara/scene/component.h"
#include "vara/scene/scene.h"

Scene scene_create(void) {
    ecs_world_t* scene_world = ecs_init();

    // Likely should register elsewhere, or have a function that does this.
    ECS_COMPONENT_DEFINE(scene_world, TransformComponent);
    ECS_COMPONENT_DEFINE(scene_world, SpriteComponent);

    return (Scene){
        .world = scene_world,
    };
}

void scene_destroy(Scene scene) {
    if (scene.world != NULL) {
        ecs_fini(scene.world);
    }
}

void scene_update(Scene scene, f32 delta) {
    ecs_world_t* world = scene.world;
    ecs_progress(world, delta);

    const ecs_query_t* sprites = ecs_query(
        world, {.terms = {{.id = ecs_id(TransformComponent)}, {.id = ecs_id(SpriteComponent)}}}
    );
    ecs_iter_t sprite_iter = ecs_query_iter(world, sprites);
    while (ecs_query_next(&sprite_iter)) {
        const TransformComponent* transform = ecs_field(&sprite_iter, TransformComponent, 0);
        const SpriteComponent* sprite = ecs_field(&sprite_iter, SpriteComponent, 1);

        for (i32 i = 0; i < sprite_iter.count; i++) {
            const Vector3 scaled_size = {
                .x = sprite->texture->width * transform->scale.x,
                .y = sprite->texture->height * transform->scale.y,
                transform->scale.z,
            };
            const Matrix4 matrix =
                mat4_mul(mat4_translation(transform->translation), mat4_scale(scaled_size));
            renderer2d_draw_sprite_matrix(matrix, sprite->texture, sprite->color, sprite->z_index);
        }
    }
}
