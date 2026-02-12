#include <vara/core/math/math.h>
#include <vara/core/platform/platform.h>

#include "vara/application/application.h"
#include "vara/renderer/buffer.h"
#include "vara/renderer/render_command.h"
#include "vara/renderer/renderer.h"
#include "vara/renderer/shader.h"
#include "vara/renderer/texture.h"
#include "vara/renderer2d/renderer2d.h"

#include "vara/renderer/font.h"
#include "vara/shaders/renderer2d_sprite.glsl.gen.h"

Renderer2D* renderer2d_create(struct Renderer* backend, const Renderer2DConfig* config) {
    return NULL;
}

void renderer2d_destroy(Renderer2D* r2d) {
}

void renderer2d_begin(Renderer2D* r2d) {
}

void renderer2d_end(Renderer2D* r2d) {
}

void renderer2d_draw_rect(
    Renderer2D* r2d, const Vector2 position, const Vector2 size, Vector4 color, i32 z_index
) {
}

void renderer2d_draw_rect_matrix(
    Renderer2D* r2d, const Matrix4 matrix, Vector4 color, i32 z_index
) {
}

void renderer2d_draw_sprite(
    Renderer2D* r2d,
    const Vector2 position,
    const Vector2 size,
    struct Texture* texture,
    Vector4 tint,
    i32 z_index
) {
}

void renderer2d_draw_sprite_matrix(
    Renderer2D* r2d, const Matrix4 matrix, struct Texture* texture, Vector4 tint, i32 z_index
) {
}

void renderer2d_draw_text(
    Renderer2D* r2d,
    const Vector2 position,
    const Vector2 size,
    const char* text,
    struct Font* font,
    Vector4 color,
    i32 z_index
) {
}

void renderer2d_draw_text_matrix(
    Renderer2D* r2d,
    const Matrix4 matrix,
    const char* text,
    struct Font* font,
    Vector4 color,
    i32 z_index
) {
}
