#pragma once

#include "mat4.h"
#include "types.h"

static Matrix4 transform2d_to_mat4(const Transform2D transform) {
    const Matrix4 mat_translation = mat4_translation((Vector3){
        .x = transform.position.x,
        .y = transform.position.y,
        0.0f,
    });
    const Matrix4 mat_scale = mat4_scale((Vector3){
        .x = transform.size.x * transform.scale.x,
        .y = transform.size.y * transform.scale.y,
        1.0f,
    });
    const Matrix4 mat_rotation = mat4_rotate_z(transform.rotation);

    const Matrix4 temp = mat4_mul(mat_translation, mat_rotation);
    const Matrix4 final = mat4_mul(temp, mat_scale);

    return final;
}