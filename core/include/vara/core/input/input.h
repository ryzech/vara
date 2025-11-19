#pragma once

#include "vara/core/defines.h"
#include "vara/core/input/keycodes.h"

b8 input_system_create(void);
void input_system_destroy(void);
void input_system_update(void);

void input_system_process_key(Key key, b8 is_pressed);

b8 input_is_key_down(Key key);
b8 input_is_key_up(Key key);