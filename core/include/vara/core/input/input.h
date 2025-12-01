#pragma once

#include "vara/core/defines.h"
#include "vara/core/input/keycodes.h"
#include "vara/core/math/types.h"

b8 input_system_create(void);
void input_system_destroy(void);
void input_system_update(void);

void input_system_process_key(Key key, b8 is_pressed);
void input_system_process_mouse(MouseButton button, b8 is_pressed);
void input_system_process_mouse_move(f32 x, f32 y);

b8 input_is_key_down(Key key);
b8 input_is_key_up(Key key);
b8 input_is_mouse_down(MouseButton button);
b8 input_is_mouse_up(MouseButton button);

Vector2 input_get_mouse_position(void);
Vector2 input_get_previous_mouse_position(void);
Vector2 input_get_mouse_position_delta(void);