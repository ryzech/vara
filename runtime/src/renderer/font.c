#include <stdio.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/font.h"

#include "vara/core/math/vec2.h"
#include "vara/renderer/texture.h"
#include "vendor/stb/stb_truetype.h"

Font* font_load(const char* path, const f32 font_size) {
    // TODO: better file loading, likely need a VFS
    FILE* font_file = fopen(path, "rb");
    if (!font_file) {
        ERROR("Failed to load font('%s') from file.", path);
        return NULL;
    }

    if (fseek(font_file, 0, SEEK_END) != 0) {
        fclose(font_file);
        ERROR("Failed to read font('%s') from file.", path);
        return NULL;
    }

    const size_t length = ftell(font_file);
    if (fseek(font_file, 0, SEEK_SET) != 0) {
        fclose(font_file);
        ERROR("Failed to read font('%s') from file.", path);
        return NULL;
    }

    u8* file_buffer = platform_allocate(length);
    const size_t read_length = fread(file_buffer, 1, length, font_file);
    if (read_length != length) {
        fclose(font_file);
        ERROR("Failed to read font('%s') from file.", path);
        return NULL;
    }
    fclose(font_file);

    Font* font = platform_allocate(sizeof(Font));
    font->size = font_size;

    // Configurable bitmap size maybe?
    const i32 width = 2048;
    const i32 height = 2048;
    u8* bitmap = platform_allocate(width * height);
    stbtt_packedchar packed_char[95];

    // Create bitmap - better method.
    stbtt_pack_context context;
    stbtt_PackBegin(&context, bitmap, width, height, 0, 1, NULL);
    stbtt_PackFontRange(&context, file_buffer, 0, font_size, 32, 95, packed_char);
    stbtt_PackEnd(&context);

    u32* pixels = platform_allocate(width * height * sizeof(u32));
    for (i32 i = 0; i < width * height; i++) {
        const u8 alpha = bitmap[i];
        pixels[i] = alpha << 24 | alpha << 16 | alpha << 8 | alpha;
    }

    const TextureConfig font_texture_config = {
        .width = width,
        .height = height,
        .format = TEXTURE_FORMAT_RGBA,
        .filter = TEXTURE_FILTER_LINEAR,
        .samples = 1,
    };
    font->atlas = texture_create(&font_texture_config);
    texture_set_data(font->atlas, pixels, width * height * sizeof(u32));

    stbtt_fontinfo info;
    stbtt_InitFont(&info, file_buffer, 0);
    i32 ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
    f32 scale = stbtt_ScaleForPixelHeight(&info, font_size);

    font->ascent = (f32)ascent * scale;
    font->descent = (f32)descent * scale;
    font->line_gap = (f32)line_gap * scale;

    for (i32 i = 0; i < 95; i++) {
        font->glyphs[i].uv_top_left.x = (f32)packed_char[i].x0 / (f32)width;
        font->glyphs[i].uv_top_left.y = (f32)packed_char[i].y0 / (f32)height;
        font->glyphs[i].uv_bottom_right.x = (f32)packed_char[i].x1 / (f32)width;
        font->glyphs[i].uv_bottom_right.y = (f32)packed_char[i].y1 / (f32)height;

        font->glyphs[i].size.x = (f32)packed_char[i].x1 - (f32)packed_char[i].x0;
        font->glyphs[i].size.y = (f32)packed_char[i].y1 - (f32)packed_char[i].y0;

        font->glyphs[i].bearing.x = packed_char[i].xoff;
        font->glyphs[i].bearing.y = packed_char[i].yoff;
        font->glyphs[i].advance = packed_char[i].xadvance;
    }

    platform_free(bitmap);
    platform_free(pixels);
    platform_free(file_buffer);

    return font;
}

void font_destroy(Font* font) {
    if (font) {
        texture_destroy(font->atlas);
        platform_free(font);
    }
}

Vector2 font_measure_text(Font* font, const char* text) {
    if (!text || !font) {
        return vec2_zero();
    }

    f32 total_width = 0.0f;
    f32 max_height = 0.0f;

    const char* ptr = text;
    while (*ptr) {
        const char c = *ptr;
        ptr++;

        if (c < 32 || c > 126) {
            continue;
        }

        const Glyph* glyph = &font->glyphs[c - 32];

        total_width += glyph->advance;
        const f32 current_glyph_height = glyph->size.y + glyph->bearing.y;
        if (current_glyph_height > max_height) {
            max_height = current_glyph_height;
        }
    }

    return (Vector2){total_width, max_height};
}
