#include <vara/core/math/math.h>

#include "vara/renderer/texture.h"
#include "vara/renderer2d/renderer2d.h"
#include "vara/scene/component.h"
#include "vara/scene/scene.h"
#include "vara/scene/system/transform_system.h"

Scene scene_create(void) {
    ecs_world_t* scene_world = ecs_init();

    // Likely should register elsewhere, or have a function that does this.
    ECS_COMPONENT_DEFINE(scene_world, TransformComponent);
    ECS_COMPONENT_DEFINE(scene_world, WorldTransformComponent);
    ECS_COMPONENT_DEFINE(scene_world, RectTransformComponent);
    ECS_COMPONENT_DEFINE(scene_world, SpriteComponent);

    // Run through nodes and update transform based on parent (if exists).
    ecs_system(
        scene_world,
        {.entity = ecs_entity(
             scene_world, {.name = "TransformSystem", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}
         ),
         .query.terms =
             {{.id = ecs_id(TransformComponent)},
              {.id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot}},
         .callback = TransformSystem}
    );

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
        world, {.terms = {{.id = ecs_id(WorldTransformComponent)}, {.id = ecs_id(SpriteComponent)}}}
    );
    ecs_iter_t sprite_iter = ecs_query_iter(world, sprites);
    while (ecs_query_next(&sprite_iter)) {
        const WorldTransformComponent* transform =
            ecs_field(&sprite_iter, WorldTransformComponent, 0);
        const SpriteComponent* sprite = ecs_field(&sprite_iter, SpriteComponent, 1);

        for (i32 i = 0; i < sprite_iter.count; i++) {
            WorldTransformComponent transform_component = transform[i];
            SpriteComponent sprite_component = sprite[i];
            if (sprite->texture != NULL) {
                renderer2d_draw_sprite_matrix(
                    transform_component.matrix,
                    sprite_component.texture,
                    sprite_component.color,
                    sprite_component.z_index
                );
            } else {
                renderer2d_draw_rect_matrix(
                    transform_component.matrix, sprite_component.color, sprite_component.z_index
                );
            }
        }
    }
}
