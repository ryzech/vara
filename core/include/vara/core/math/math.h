#pragma once

#include "vara/core/math/types.h"

#define PI            3.14159265358979323846264338327950288
#define PI_OVER_TWO   PI / 2
#define PI_OVER_FOUR  PI / 4
#define ONE_OVER_PI   1 / PI
#define TWO_OVER_PI   2 / PI

#define TAU           PI * 2
#define TAU_OVER_TWO  TAU / 2
#define TAU_OVER_FOUR TAU / 4
#define ONE_OVER_TAU  1 / TAU
#define TWO_OVER_TAU  2 / TAU

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
