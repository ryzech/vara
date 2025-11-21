#pragma once

#include "types.h"

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