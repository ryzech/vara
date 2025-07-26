#pragma once

#include "core/defines.h"

typedef struct platform_state {
	void *internal_state;
} platform_state;

b8 platform_init(platform_state *state, const char *application_name, i32 x,
				 i32 y, i32 width, i32 height);
void platform_destroy(platform_state *state);

b8 platform_poll_event(platform_state *state);
f64 platform_get_time(void);

void *platform_allocate(u64 size, b8 aligned);
void platform_free(void *block, b8 aligned);
void *platform_zero_memory(void *block, u64 size);
void *platform_copy_memory(void *dst, const void *src, u64 size);
void *platform_set_memory(void *block, i32 value, u64 size);

void platform_write(const char *message);
void platform_error(const char *message);

f64 platform_get_time(void);

void platform_sleep(u64 milliseconds);
