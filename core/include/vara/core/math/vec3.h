#pragma once

#include <math.h>
#include "types.h"

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

static f32 vec3_length(Vector3 vector) {
    return sqrtf(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

static i32 vec3i_length(Vector3i vector) {
    return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
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

static Vector3i vec3i_normalize(Vector3i vector) {
    i32 length = vec3i_length(vector);
    if (length == 0) {
        return vec3i_zero();
    }
    return (Vector3i){
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

static Vector3i vec3i_cross(Vector3i a, Vector3i b) {
    return (Vector3i){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

static f32 vec3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static i32 vec3i_dot(Vector3i a, Vector3i b) {
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