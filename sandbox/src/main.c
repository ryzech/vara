#include <stdio.h>
#include <stdlib.h>

#include <core/core.h>
#include <core/defines.h>
#include <core/logger.h>
#include <core/platform/platform.h>

u64 sleep_time = 5;

i32 main(void) {
    DEBUG("Version: %s", VARA_VERSION);

    DEBUG("Absolute time: %f", platform_get_time());
    DEBUG("Sleeping for %lld ms...", sleep_time);
    platform_sleep(sleep_time);
    DEBUG("Absolute time: %f", platform_get_time());

    return EXIT_SUCCESS;
}
