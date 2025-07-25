#include <stdio.h>
#include <stdlib.h>

#include <archimedes/archimedes.h>
#include <core/defines.h>
#include <core/logger.h>

int main(void) {
	FATAL("Version: %s", _VERSION);
	ERROR("Version: %s", _VERSION);
	WARN("Version: %s", _VERSION);
	INFO("Version: %s", _VERSION);
	DEBUG("Version: %s", _VERSION);
	TRACE("Version: %s", _VERSION);
	return EXIT_SUCCESS;
}
