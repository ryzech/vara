#include "core/platform/platform.h"

#if defined(_PLATFORM_APPLE)

#include <memory.h>
#include <stdio.h>

#include <mach/mach_time.h>

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

f64 platform_get_time(void) {
	mach_timebase_info_data_t clock_timebase;
	mach_timebase_info(&clock_timebase);

	u64 mach_absolute = mach_absolute_time();

	f64 seconds = (f64)mach_absolute * (f64)clock_timebase.numer /
				  ((f64)clock_timebase.denom * 1.0e9);
	return seconds;
}

void *platform_allocate(u64 size, __unused b8 aligned) { return malloc(size); }

void platform_free(void *block, __unused b8 aligned) { free(block); }

void *platform_zero_memory(void *block, u64 size) {
	return memset(block, 0, size);
}

void *platform_copy_memory(void *dst, const void *src, u64 size) {
	return memcpy(dst, src, size);
}

void *platform_set_memory(void *dst, i32 value, u64 size) {
	return memset(dst, value, size);
}

void platform_write(const char *message) { printf("%s", message); }

void platform_error(const char *message) { printf("%s", message); }

#endif
