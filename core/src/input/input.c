#include "vara/core/input/input.h"

#include "vara/core/input/keycodes.h"
#include "vara/core/platform/platform.h"

typedef struct KeyState KeyState;
typedef struct MouseButtonState MouseButtonState;
typedef struct InputState InputState;

struct KeyState {
    b8 is_pressed[KEY_MAX];
};

struct InputState {
    KeyState current_key_state;
    KeyState previous_key_state;
};

static InputState input_state;

b8 input_system_create(void) {
    platform_allocate(sizeof(input_state));
    platform_zero_memory(&input_state, sizeof(input_state));
    return true;
}

void input_system_destroy(void) {
    platform_zero_memory(&input_state, sizeof(input_state));
}

void input_system_update(void) {
    platform_copy_memory(
        &input_state.previous_key_state,
        &input_state.current_key_state,
        sizeof(KeyState)
    );
}

void input_system_process_key(const Key key, const b8 is_pressed) {
    const b8 changed =
        input_state.current_key_state.is_pressed[key] != is_pressed;
    if (changed) {
        input_state.current_key_state.is_pressed[key] = is_pressed;
    }
}

b8 input_is_key_down(Key key) {
    return input_state.current_key_state.is_pressed[key];
}

b8 input_is_key_up(Key key) {
    return !input_state.current_key_state.is_pressed[key];
}
