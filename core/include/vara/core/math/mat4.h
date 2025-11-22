#pragma once

#include "types.h"
#include "vara/core/platform/platform.h"
#include "vec3.h"

/**
 * Default state of a Matrix.
 * @return Identity matrix.
 */
static Matrix4 mat4_identity(void) {
    Matrix4 out;
    platform_zero_memory(out.elements, sizeof(Matrix4));
    out.elements[0] = 1.0f;
    out.elements[5] = 1.0f;
    out.elements[10] = 1.0f;
    out.elements[15] = 1.0f;
    return out;
}

static Matrix4 mat4_look_at(Vector3 position, Vector3 target, Vector3 up) {
    Matrix4 out = mat4_identity();

    Vector3 z_axis = vec3_normalize(vec3_sub(target, position));
    Vector3 x_axis = vec3_normalize(vec3_cross(z_axis, up));
    Vector3 y_axis = vec3_cross(x_axis, z_axis);

    out.elements[0] = x_axis.x;
    out.elements[1] = y_axis.x;
    out.elements[2] = -z_axis.x;
    out.elements[3] = 0.0f;

    out.elements[4] = x_axis.y;
    out.elements[5] = y_axis.y;
    out.elements[6] = -z_axis.y;
    out.elements[7] = 0.0f;

    out.elements[8] = x_axis.z;
    out.elements[9] = y_axis.z;
    out.elements[10] = -z_axis.z;
    out.elements[11] = 0.0f;

    out.elements[12] = -vec3_dot(x_axis, position);
    out.elements[13] = -vec3_dot(y_axis, position);
    out.elements[14] = vec3_dot(z_axis, position);
    out.elements[15] = 1.0f;

    return out;
}

static Matrix4 mat4_perspective(
    f32 fov, f32 aspect, f32 near_clip, f32 far_clip
) {
    Matrix4 out = mat4_identity();

    f32 tan_half_fov = tan(fov * 0.5);
    f32 range_inv = 1.0f / (near_clip - far_clip);

    out.elements[0] = 1.0f / (aspect * tan_half_fov);
    out.elements[5] = 1.0f / tan_half_fov;
    out.elements[10] = (far_clip + near_clip) * range_inv;
    out.elements[11] = -1.0f;
    out.elements[14] = 2.0f * far_clip * near_clip * range_inv;

    return out;
}

static Matrix4 mat4_mul(Matrix4 a, Matrix4 b) {
    Matrix4 out = mat4_identity();

    // TODO: optimize this further.
    for (i32 c = 0; c < 4; c++) {
        for (i32 r = 0; r < 4; r++) {
            f32 sum = 0.0f;
            for (i32 i = 0; i < 4; i++) {
                sum += a.elements[i * 4 + r] * b.elements[c * 4 + i];
            }
            out.elements[c * 4 + r] = sum;
        }
    }

    return out;
}
