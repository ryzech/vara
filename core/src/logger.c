#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "core/logger.h"
#include "core/platform/platform.h"

b8 initialize_logging(void) {
	// Do something here.
	return true;
}

void shutdown_logging(void) {
	// Do something here.
}

void _log_output(log_level level, const char *message, ...) {
	const char *level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ",
									"[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};

	char out_message[32000];
	platform_set_memory(out_message, 0, sizeof(out_message));

	__builtin_va_list arg_ptr;
	va_start(arg_ptr, message);
	vsnprintf(out_message, sizeof(out_message), message, arg_ptr);
	va_end(arg_ptr);

	char final_message[32000];
	sprintf(final_message, "%s%s\n", level_strings[level], out_message);
	platform_write(final_message);
}
