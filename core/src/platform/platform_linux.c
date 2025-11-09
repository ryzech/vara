#include "vara/core/platform/platform.h"

#ifdef VARA_PLATFORM_LINUX

#include <GLFW/glfw3.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>

#include "vara/core/defines.h"
#include "vara/core/logger.h"

b8 platform_create(void) {
    DEBUG("Initializing glfw %s", glfwGetVersionString());
    if (!glfwInit()) {
        return false;
    }

    return true;
}

void platform_destroy(void) {
    glfwTerminate();
}

b8 platform_poll_events(void) {
    glfwPollEvents();
    return true;
}

void* platform_allocate(u64 size) {
    return malloc(size);
}

void platform_free(void* block) {
    free(block);
}

void* platform_zero_memory(void* block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dst, const void* src, u64 size) {
    return memcpy(dst, src, size);
}

void* platform_set_memory(void* dst, i32 value, u64 size) {
    return memset(dst, value, size);
}

void platform_write(const char* message) {
    printf("%s", message);
}

void platform_error(const char* message) {
    printf("%s", message);
}

f64 platform_get_time() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);

    f64 seconds = (f64)time.tv_sec + (f64)time.tv_nsec / 1.0e9;
    return seconds;
}

#endif
