#include "core/platform/platform.h"

#ifdef VARA_PLATFORM_APPLE

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <Security/SecRandom.h>
#import <mach/mach_time.h>

#include <GLFW/glfw3.h>
#include <memory.h>
#include <stdio.h>

#include "core/logger.h"

@class ApplicationDelegate;
@class WindowDelegate;
@class ContentView;

b8 platform_create(void) {
    // TODO: should have a method on each platform to get the version, product name, and build rev (if possible.)
    @autoreleasepool {
        NSOperatingSystemVersion version =
            [[NSProcessInfo processInfo] operatingSystemVersion];
        DEBUG(
            "Initializing platform - macOS (%d.%d.%d)",
            version.majorVersion,
            version.minorVersion,
            version.patchVersion
        );
    }
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

void* platform_allocate(u64 size, __unused b8 aligned) {
    return malloc(size);
}

void platform_free(void* block, __unused b8 aligned) {
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

b8 platform_get_random_bytes(void* buf, size_t buf_size) {
    int result = SecRandomCopyBytes(kSecRandomDefault, buf_size, buf);
    return result == 0;
}

void platform_write(const char* message) {
    printf("%s", message);
}

void platform_error(const char* message) {
    printf("%s", message);
}

f64 platform_get_time(void) {
    mach_timebase_info_data_t clock_timebase;
    mach_timebase_info(&clock_timebase);

    u64 mach_absolute = mach_absolute_time();

    f64 seconds = (f64)mach_absolute * (f64)clock_timebase.numer
                  / ((f64)clock_timebase.denom * 1.0e9);
    return seconds;
}

void platform_sleep(u64 milliseconds) {
    usleep(milliseconds * 1000);
}

#endif
