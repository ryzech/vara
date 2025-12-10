#pragma once

#include <flecs.h>
#include <vara/core/math/types.h>

typedef struct TransformComponent TransformComponent;

struct TransformComponent {
    Vector3 translation;
    Vector3 rotation;
    Vector3 scale;
};

// Declare components so the type is accessible.
// Different than registering.
ECS_COMPONENT_DECLARE(TransformComponent);