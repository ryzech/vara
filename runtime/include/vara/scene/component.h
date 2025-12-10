#pragma once

#include <flecs.h>
#include <vara/core/math/types.h>

typedef struct TransformComponent TransformComponent;

struct TransformComponent {
    Matrix4 transform;
};

// Declare components so the type is accessible.
// Different than registering.
ECS_COMPONENT_DECLARE(TransformComponent);