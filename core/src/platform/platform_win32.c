#include "vara/core/platform/platform.h"

#ifdef VARA_PLATFORM_WINDOWS

#include <windows.h>
#include <GLFW/glfw3.h>
#include <memory.h>
#include <stdio.h>

#include "vara/core/defines.h"
#include "vara/core/logger.h"

static f64 clock_frequency;

void clock_setup(void) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (f64)frequency.QuadPart;
}

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
    // Can replace with ConsoleWriteA/W later if needed.
    printf("%s", message);
}

void platform_error(const char* message) {
    // Can replace with ConsoleWriteA/W later if needed.
    printf("%s", message);
}

f64 platform_get_time() {
    // Need a separate function as we can't set this every frame
    // It gets weird.
    // Thanks Travis Vroman (Kohi) for this portion of code/idea.
    if (!clock_frequency) {
        clock_setup();
    }

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return (f64)time.QuadPart * clock_frequency;
}

#endif
