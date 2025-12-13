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

static Matrix4 mat4_perspective(f32 fov, f32 aspect, f32 near_clip, f32 far_clip) {
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

static Matrix4 mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    Matrix4 out = mat4_identity();

    out.elements[0] = 2.0f / (right - left);
    out.elements[5] = 2.0f / (top - bottom);
    out.elements[10] = -2.0f / (far - near);
    out.elements[12] = -(right + left) / (right - left);
    out.elements[13] = -(top + bottom) / (top - bottom);
    out.elements[14] = -(far + near) / (far - near);

    return out;
}

static Matrix4 mat4_translation(const Vector3 position) {
    Matrix4 out = mat4_identity();

    out.elements[12] = position.x;
    out.elements[13] = position.y;
    out.elements[14] = position.z;

    return out;
}

static Matrix4 mat4_scale(const Vector3 scale) {
    Matrix4 out = mat4_identity();

    out.elements[0] = scale.x;
    out.elements[5] = scale.y;
    out.elements[10] = scale.z;

    return out;
}

static Matrix4 mat4_rotate_x(const f32 rotation) {
    Matrix4 out = mat4_identity();

    const f32 cos_value = cos(rotation);
    const f32 sin_value = sin(rotation);

    out.elements[5] = cos_value;
    out.elements[6] = sin_value;
    out.elements[9] = -sin_value;
    out.elements[10] = cos_value;

    return out;
}

static Matrix4 mat4_rotate_y(const f32 rotation) {
    Matrix4 out = mat4_identity();

    const f32 cos_value = cos(rotation);
    const f32 sin_value = sin(rotation);

    out.elements[0] = cos_value;
    out.elements[2] = -sin_value;
    out.elements[8] = sin_value;
    out.elements[10] = cos_value;

    return out;
}

static Matrix4 mat4_rotate_z(const f32 rotation) {
    Matrix4 out = mat4_identity();

    const f32 cos_value = cos(rotation);
    const f32 sin_value = sin(rotation);

    out.elements[0] = cos_value;
    out.elements[1] = sin_value;
    out.elements[4] = -sin_value;
    out.elements[5] = cos_value;

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

static Matrix4 mat4_rotate(const Vector3 rotation) {
    const Matrix4 rotated_x = mat4_rotate_x(rotation.x);
    const Matrix4 rotated_y = mat4_rotate_y(rotation.x);
    const Matrix4 rotated_z = mat4_rotate_z(rotation.z);
    const Matrix4 rotated_xy = mat4_mul(rotated_x, rotated_y);
    const Matrix4 out = mat4_mul(rotated_xy, rotated_z);

    return out;
}

static Vector3 mat4_mul_vec3(const Matrix4 matrix, const Vector3 vec) {
    return (Vector3){
        .x = vec.x * matrix.elements[0] + vec.y * matrix.elements[4] + vec.z * matrix.elements[8]
             + matrix.elements[12],
        .y = vec.x * matrix.elements[1] + vec.y * matrix.elements[5] + vec.z * matrix.elements[9]
             + matrix.elements[13],
        .z = vec.x * matrix.elements[2] + vec.y * matrix.elements[6] + vec.z * matrix.elements[10]
             + matrix.elements[14],
    };
}
