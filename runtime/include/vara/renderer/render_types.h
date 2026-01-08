#pragma once

#include <vara/core/defines.h>

typedef enum CompareOp CompareOp;
typedef enum BlendFactor BlendFactor;
typedef struct DepthState DepthState;
typedef struct BlendState BlendState;

enum CompareOp {
    COMPARE_NEVER,
    COMPARE_LESS,
    COMPARE_EQUAL,
    COMPARE_LESS_OR_EQUAL,
    COMPARE_GREATER,
    COMPARE_NOT_EQUAL,
    COMPARE_GREATER_OR_EQUAL,
    COMPARE_ALWAYS
};

enum BlendFactor {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_ONE_MINUS_SRC_COLOR,
    BLEND_DST_COLOR,
    BLEND_ONE_MINUS_DST_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_ONE_MINUS_SRC_ALPHA,
    BLEND_DST_ALPHA,
    BLEND_ONE_MINUS_DST_ALPHA
};

struct DepthState {
    b8 test, write;
    CompareOp compare;
};

struct BlendState {
    b8 enable;
    BlendFactor src, dst;
};
