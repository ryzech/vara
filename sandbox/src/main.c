#include <stdio.h>
#include <stdlib.h>

#include <core/core.h>
#include <core/defines.h>
#include <core/platform/platform.h>
#include <core/logger.h>

i32 main(void) {
	DEBUG("Version: %s", _VERSION);
	DEBUG("Absolute time: %f", platform_get_time());

	return EXIT_SUCCESS;
}
