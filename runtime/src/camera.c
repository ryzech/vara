#include "vara/camera/camera.h"

#include "vara/core/platform/platform.h"

// Should this be exposed in the API?
// For now, I'll keep it static to the implementation.
static void camera_recalculate_view(Camera* camera) {
    const Vector3 forward = (Vector3){
        0.0f,
        0.0f,
        -1.0f,
    };
    const Vector3 target = vec3_add(camera->position, forward);
    const Vector3 up = (Vector3){
        0.0f,
        1.0f,
        0.0f,
    };

    camera->view = mat4_look_at(camera->position, target, up);
}

Camera* camera_create(void) {
    Camera* camera = platform_allocate(sizeof(Camera));
    if (!camera) {
        return NULL;
    }

    camera->position = vec3_zero();
    camera->view = mat4_identity();
    camera->projection = mat4_identity();

    return camera;
}

void camera_destroy(Camera* camera) {
    platform_free(camera);
}

Vector3 camera_get_position(const Camera* camera) {
    return camera->position;
}

Matrix4 camera_get_view(const Camera* camera) {
    return camera->view;
}

Matrix4 camera_get_projection(const Camera* camera) {
    return camera->projection;
}

void camera_set_position(Camera* camera, const Vector3 position) {
    camera->position = position;
    camera_recalculate_view(camera);
}

void camera_move(Camera* camera, Vector3 delta) {
    camera->position = vec3_add(camera->position, delta);
    camera_recalculate_view(camera);
}
