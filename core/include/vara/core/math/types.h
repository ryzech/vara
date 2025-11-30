#pragma once

#include "vara/core/defines.h"

typedef union vector2_u {
    /** An array of x, y */
    f32 elements[2];

    struct {
        /** The first element */
        f32 x;
        /** The second element */
        f32 y;
    };
} Vector2;

typedef union vector3_u {
    /** An array of x, y, z */
    f32 elements[3];

    struct {
        /** The first element */
        f32 x;
        /** The second element */
        f32 y;
        /** The third element */
        f32 z;
    };
} Vector3;

typedef union vector4_u {
    /** An array of x, y, z, w */
    f32 elements[4];

    struct {
        /** The first element */
        f32 x;
        /** The second element */
        f32 y;
        /** The third element */
        f32 z;
        /** The fourth element */
        f32 w;
    };
} Vector4;

typedef union vector2i_u {
    /** An array of x, y */
    i32 elements[2];

    struct {
        /** The first element */
        i32 x;
        /** The second element */
        i32 y;
    };
} Vector2i;

typedef union vector3i_u {
    /** An array of x, y, z */
    i32 elements[3];

    struct {
        /** The first element */
        i32 x;
        /** The second element */
        i32 y;
        /** The third element */
        i32 z;
    };
} Vector3i;

typedef union vector4i_u {
    /** An array of x, y, z, w */
    i32 elements[4];

    struct {
        /** The first element */
        i32 x;
        /** The second element */
        i32 y;
        /** The third element */
        i32 z;
        /** The fourth element */
        i32 w;
    };
} Vector4i;

typedef Vector4 Quaternion;

typedef union matrix4_u {
    f32 elements[16];
} Matrix4;

typedef struct Vertex {
    Vector3 position;
    Vector3 color;
    Vector2 tex_coord;
    f32 tex_index;
} Vertex;

typedef struct Rect {
    Vector3 position;
    Vector3 size;
} Rect;
