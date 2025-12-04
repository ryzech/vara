#include <stdio.h>
#include <vara/core/logger.h>
#include <vara/core/platform/platform.h>

#include "vara/renderer/font.h"
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
        ERROR("Failed to read font('%s') from file.", path);
        return NULL;
    }

    const size_t length = ftell(font_file);
    if (fseek(font_file, 0, SEEK_SET) != 0) {
        ERROR("Failed to read font('%s') from file.", path);
        return NULL;
    }

    unsigned char* file_buffer = platform_allocate(length);
    const size_t read_length = fread(file_buffer, 1, length, font_file);
    if (read_length != length) {
        ERROR("Failed to read font('%s') from file.", path);
        return NULL;
    }

    Font* font = platform_allocate(sizeof(Font));
    font->size = font_size;

    const i32 width = 512;
    const i32 height = 512;
    unsigned char* bitmap = platform_allocate(width * height);
    stbtt_bakedchar char_data[95];
    stbtt_BakeFontBitmap(file_buffer, 0, font_size, bitmap, width, height, 32, 95, char_data);

    u32* pixels = platform_allocate(width * height * sizeof(u32));
    for (i32 i = 0; i < width * height; i++) {
        const u8 alpha = bitmap[i];
        pixels[i] = (alpha << 24) | (255 << 16) | (255 << 8) | 255;
    }

    const TextureConfig font_texture_config = {
        .width = width,
        .height = height,
        .format = TEXTURE_FORMAT_RGBA,
        .filter = TEXTURE_FILTER_LINEAR,
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
        font->glyphs[i].uv_top_left.x = (f32)char_data[i].x0 / (f32)width;
        font->glyphs[i].uv_top_left.y = (f32)char_data[i].y0 / (f32)height;
        font->glyphs[i].uv_bottom_right.x = (f32)char_data[i].x1 / (f32)width;
        font->glyphs[i].uv_bottom_right.y = (f32)char_data[i].y1 / (f32)height;

        font->glyphs[i].size.x = (f32)char_data[i].x1 - (f32)char_data[i].x0;
        font->glyphs[i].size.y = (f32)char_data[i].y1 - (f32)char_data[i].y0;

        font->glyphs[i].bearing.x = char_data[i].xoff;
        font->glyphs[i].bearing.y = char_data[i].yoff;
        font->glyphs[i].advance = char_data[i].xadvance;
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
