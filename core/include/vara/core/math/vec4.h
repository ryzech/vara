#pragma once

#include "types.h"

// Constructors

static Vector4 vec4(f32 x, f32 y, f32 z, f32 w) {
    return (Vector4){
        x,
        y,
        z,
        w,
    };
}

static Vector4i vec4i(i32 x, i32 y, i32 z, i32 w) {
    return (Vector4i){
        x,
        y,
        z,
        w,
    };
}