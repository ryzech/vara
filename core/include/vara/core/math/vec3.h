#pragma once

#include <math.h>
#include "types.h"

// Conversion helpers

static Vector3 vec3_from_i(const Vector3i vec) {
    return (Vector3){
        (f32)vec.x,
        (f32)vec.y,
        (f32)vec.z,
    };
}

static Vector3i vec3i_from_f(const Vector3 vec) {
    return (Vector3i){
        (i32)vec.x,
        (i32)vec.y,
        (i32)vec.z,
    };
}

// Constants

static Vector3 vec3_zero(void) {
    return (Vector3){
        0,
        0,
        0,
    };
}

static Vector3i vec3i_zero(void) {
    return (Vector3i){
        0,
        0,
        0,
    };
}

static Vector3 vec3_one(void) {
    return (Vector3){
        1.0f,
        1.0f,
        1.0f,
    };
}

static Vector3i vec3i_one(void) {
    return (Vector3i){
        1,
        1,
        1,
    };
}

static Vector3 vec3_up(void) {
    return (Vector3){
        0.0f,
        1.0f,
        0.0f,
    };
}

static Vector3i vec3i_up(void) {
    return (Vector3i){
        0,
        1,
        0,
    };
}

static Vector3 vec3_down(void) {
    return (Vector3){
        0.0f,
        -1.0f,
        0.0f,
    };
}

static Vector3i vec3i_down(void) {
    return (Vector3i){
        0,
        -1,
        0,
    };
}

static Vector3 vec3_left(void) {
    return (Vector3){
        -1.0f,
        0.0f,
        0.0f,
    };
}

static Vector3i vec3i_left(void) {
    return (Vector3i){
        -1,
        0,
        0,
    };
}

static Vector3 vec3_right(void) {
    return (Vector3){
        1.0f,
        0.0f,
        0.0f,
    };
}

static Vector3i vec3i_right(void) {
    return (Vector3i){
        1,
        0,
        0,
    };
}

static Vector3 vec3_forward(void) {
    return (Vector3){
        0.0f,
        0.0f,
        -1.0f,
    };
}

static Vector3i vec3i_forward(void) {
    return (Vector3i){
        0,
        0,
        -1,
    };
}

static Vector3 vec3_backward(void) {
    return (Vector3){
        0.0f,
        0.0f,
        1.0f,
    };
}

static Vector3i vec3i_backward(void) {
    return (Vector3i){
        0,
        0,
        1,
    };
}

// Functions

static f32 vec3_length(Vector3 vector) {
    return sqrtf(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

static Vector3 vec3_normalize(Vector3 vector) {
    f32 length = vec3_length(vector);
    if (length == 0) {
        return vec3_zero();
    }
    return (Vector3){
        vector.x /= length,
        vector.y /= length,
        vector.z /= length,
    };
}

static Vector3 vec3_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

static f32 vec3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vector3 vec3_add(Vector3 a, Vector3 b) {
    return (Vector3){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

static Vector3i vec3i_add(Vector3i a, Vector3i b) {
    return (Vector3i){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

static Vector3 vec3_sub(Vector3 a, Vector3 b) {
    return (Vector3){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}

static Vector3i vec3i_sub(Vector3i a, Vector3i b) {
    return (Vector3i){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}

static Vector3 vec3_mul(Vector3 a, Vector3 b) {
    return (Vector3){
        a.x * b.x,
        a.y * b.y,
        a.z * b.z,
    };
}

static Vector3i vec3i_mul(Vector3i a, Vector3i b) {
    return (Vector3i){
        a.x * b.x,
        a.y * b.y,
        a.z * b.z,
    };
}

static Vector3 vec3_div(Vector3 a, Vector3 b) {
    return (Vector3){
        a.x / b.x,
        a.y / b.y,
        a.z / b.z,
    };
}

static Vector3i vec3i_div(Vector3i a, Vector3i b) {
    return (Vector3i){
        a.x / b.x,
        a.y / b.y,
        a.z / b.z,
    };
}