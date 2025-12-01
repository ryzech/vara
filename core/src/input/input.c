#include "vara/core/input/input.h"
#include "vara/core/platform/platform.h"

typedef struct KeyState KeyState;
typedef struct MouseState MouseState;
typedef struct InputState InputState;

struct KeyState {
    b8 is_pressed[KEY_MAX];
};

struct MouseState {
    f32 x, y;
    b8 is_pressed[MOUSE_BUTTON_MAX];
};

struct InputState {
    KeyState current_key_state;
    KeyState previous_key_state;
    MouseState current_mouse_state;
    MouseState previous_mouse_state;
};

static InputState* input_state;

b8 input_system_create(void) {
    input_state = platform_allocate(sizeof(InputState));
    platform_zero_memory(input_state, sizeof(InputState));
    return true;
}

void input_system_destroy(void) {
    platform_free(input_state);
}

void input_system_update(void) {
    platform_copy_memory(
        &input_state->previous_key_state, &input_state->current_key_state, sizeof(KeyState)
    );
    platform_copy_memory(
        &input_state->previous_mouse_state, &input_state->current_mouse_state, sizeof(MouseState)
    );
}

void input_system_process_key(const Key key, const b8 is_pressed) {
    const b8 changed = input_state->current_key_state.is_pressed[key] != is_pressed;
    if (changed) {
        input_state->current_key_state.is_pressed[key] = is_pressed;
    }
}

void input_system_process_mouse(MouseButton button, b8 is_pressed) {
    const b8 changed = input_state->current_mouse_state.is_pressed[button] != is_pressed;
    if (changed) {
        input_state->current_mouse_state.is_pressed[button] = is_pressed;
    }
}

void input_system_process_mouse_move(f32 x, f32 y) {
    const b8 changed =
        input_state->current_mouse_state.x != x || input_state->current_mouse_state.y != y;
    if (changed) {
        input_state->current_mouse_state.x = x;
        input_state->current_mouse_state.y = y;
    }
}

b8 input_is_key_down(Key key) {
    return input_state->current_key_state.is_pressed[key];
}

b8 input_is_key_up(Key key) {
    return !input_state->current_key_state.is_pressed[key];
}

b8 input_is_mouse_down(MouseButton button) {
    return input_state->current_mouse_state.is_pressed[button];
}

b8 input_is_mouse_up(MouseButton button) {
    return !input_state->current_mouse_state.is_pressed[button];
}

Vector2 input_get_mouse_position(void) {
    return (Vector2){
        .x = input_state->current_mouse_state.x,
        .y = input_state->current_mouse_state.y,
    };
}

Vector2 input_get_previous_mouse_position(void) {
    return (Vector2){
        .x = input_state->previous_mouse_state.x,
        .y = input_state->previous_mouse_state.y,
    };
}

Vector2 input_get_mouse_position_delta(void) {
    return (Vector2){
        .x = input_state->current_mouse_state.x - input_state->previous_mouse_state.x,
        .y = input_state->current_mouse_state.y - input_state->previous_mouse_state.y,
    };
}
