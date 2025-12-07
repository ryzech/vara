#pragma once

#include "vara/core/math/types.h"

static Vector2 vec2_zero(void) {
    return (Vector2){
        0,
        0,
    };
}

static Vector2i vec2i_zero(void) {
    return (Vector2i){
        0,
        0,
    };
}

static Vector2 vec2_one(void) {
    return (Vector2){
        1,
        1,
    };
}

static Vector2i vec2i_one(void) {
    return (Vector2i){
        1,
        1,
    };
}

static Vector2 vec2_mul(Vector2 a, Vector2 b) {
    return (Vector2){
        a.x * b.x,
        a.y * b.y,
    };
}

static Vector2i vec2i_mul(Vector2i a, Vector2i b) {
    return (Vector2i){
        a.x * b.x,
        a.y * b.y,
    };
}

static Vector2 vec2_div(Vector2 a, Vector2 b) {
    return (Vector2){
        a.x / b.x,
        a.y / b.y,
    };
}

static Vector2i vec2i_div(Vector2i a, Vector2i b) {
    return (Vector2i){
        a.x / b.x,
        a.y / b.y,
    };
}