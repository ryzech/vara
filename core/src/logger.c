#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "vara/core/logger.h"
#include "vara/core/platform/platform.h"

// How should we handle this?
// Platform DEBUG logs before level is set.
// Should we default this to DEBUG or handle it earlier?
static LogLevel global_level = LOG_LEVEL_DEBUG;

b8 logging_system_create(LogLevel level) {
    // TODO: add rolling log files.
    global_level = level;
    return true;
}

void logging_system_destroy(void) {
    // Do something here.
}

void _log_output(const LogLevel level, const char* func, const char* message, ...) {
    if (!message) {
        return;
    }

    if (level > global_level) {
        return;
    }

    const char* level_strings[6] = {
        "fatal",
        "error",
        "warn",
        "info",
        "debug",
        "trace",
    };

    char out_message[4096];
    platform_set_memory(out_message, 0, sizeof(out_message));

    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, sizeof(out_message), message, arg_ptr);
    va_end(arg_ptr);

    char final_message[4200];
    platform_set_memory(final_message, 0, sizeof(final_message));
    snprintf(
        final_message,
        sizeof(final_message),
        "%s(%s): %s\n",
        level_strings[level],
        func,
        out_message
    );
    platform_write(final_message);
}
