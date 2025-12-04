#pragma once

#include <vara/core/defines.h>
#include <vara/core/math/types.h>

typedef struct Glyph Glyph;
typedef struct Font Font;

// Forward declaration.
struct Texture;

struct Glyph {
    Vector2 uv_top_left;
    Vector2 uv_bottom_right;
    Vector2 size;
    Vector2 bearing;
    f32 advance;
};

struct Font {
    struct Texture* atlas;
    // Do we need to support more than 256?
    Glyph glyphs[95];
    f32 size;
    f32 ascent;
    f32 descent;
    f32 line_gap;
};

Font* font_load(const char* path, const f32 font_size);
void font_destroy(Font* font);
