#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/math.h>

/**
 * Camera.
 */
typedef struct Camera Camera;

struct Camera {
    Vector3 position;
    Matrix4 view;
    Matrix4 projection;
};

/**
 * Creates and allocates new Camera with all values zeroed.
 * @return New Camera.
 */
Camera* camera_create(void);
/**
 * Frees and destroys given Camera.
 * @param camera Camera to destroy.
 */
void camera_destroy(Camera* camera);

/**
 * Returns zeroed Vector3 if not set.
 * @param camera Camera to get position of.
 * @return A Vector3 of the Camera's current position.
 */
Vector3 camera_get_position(Camera* camera);
/**
 * Returns and identity matrix if not set.
 * @param camera Camera to get view matrix of.
 * @return Current view matrix of the Camera.
 */
Matrix4 camera_get_view(Camera* camera);
/**
 * Returns an identity matrix if not set.
 * @param camera Camera to get projection matrix of.
 * @return Current projection matrix of the camera.
 */
Matrix4 camera_get_projection(const Camera* camera);

/**
 * Set the position Vector of the Camera.
 * Does not update the matrices.
 * @param camera Camera to set position of.
 * @param position Vector3 containing the new position.
 */
void camera_set_position(Camera* camera, Vector3 position);
/**
 * This will update the matrices according to the new position.
 * @param camera Camera to move.
 * @param delta Vector containing the delta to move towards.
 */
void camera_move(Camera* camera, Vector3 delta);