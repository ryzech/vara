#pragma once

#include "core/defines.h"

typedef union vector2_u Vector2;
typedef union vector3_u Vector3;
typedef union vector4_u Vector4;

typedef union vector2i_u Vector2i;
typedef union vector3i_u Vector3i;
typedef union vector4i_u Vector4i;

union vector2_u {
    /* An array of x, y */
    f32 elements[2];

    struct {
        /* The first element */
        f32 x;
        /* The second element */
        f32 y;
    };
};

union vector3_u {
    /* An array of x, y, z */
    f32 elements[3];

    struct {
        /* The first element */
        f32 x;
        /* The second element */
        f32 y;
        /* The second element */
        f32 z;
    };
};

union vector4_u {
    /* An array of x, y, z, w */
    f32 elements[4];

    struct {
        /* The first element */
        f32 x;
        /* The second element */
        f32 y;
        /* The second element */
        f32 z;
        /* The second element */
        f32 w;
    };
};

union vector2i_u {
    /* An array of x, y */
    i32 elements[2];

    struct {
        /* The first element */
        i32 x;
        /* The second element */
        i32 y;
    };
};

union vector3i_u {
    /* An array of x, y, z */
    i32 elements[3];

    struct {
        /* The first element */
        i32 x;
        /* The second element */
        i32 y;
        /* The second element */
        i32 z;
    };
};

union vector4i_u {
    /* An array of x, y, z, w */
    i32 elements[4];

    struct {
        /* The first element */
        i32 x;
        /* The second element */
        i32 y;
        /* The second element */
        i32 z;
        /* The second element */
        i32 w;
    };
};
