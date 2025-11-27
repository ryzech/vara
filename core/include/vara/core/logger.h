#pragma once

#include "vara/core/defines.h"

/** Indicates if warning log level is enabled. */
#define LOG_WARN_ENABLED 1
/** Indicates if info log level is enabled. */
#define LOG_INFO_ENABLED 1

/** Disable LOG_TRACE and LOG_DEBUG if _RELEASE */
#if defined(VARA_RELEASE)
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#else
/** Indicates if debug log level is enabled. */
#define LOG_DEBUG_ENABLED 1
/** Indicates if trace log level is enabled. */
#define LOG_TRACE_ENABLED 1
#endif

/** @brief Represents levels of logging */
typedef enum LogLevel {
    /** @brief Fatal log level, should be used to stop the application when hit.
	 */
    LOG_LEVEL_FATAL = 0,
    /** @brief Error log level, should be used to indicate critical runtime
	   problems that cause the application to run improperly or not at all. */
    LOG_LEVEL_ERROR = 1,
    /** @brief Warning log level, should be used to indicate non-critial
	   problems with the application that cause it to run suboptimally. */
    LOG_LEVEL_WARN = 2,
    /** @brief Info log level, should be used for non-erronuous informational
	   purposes. */
    LOG_LEVEL_INFO = 3,
    /** @brief Debug log level, should be used for debugging purposes. */
    LOG_LEVEL_DEBUG = 4,
    /** @brief Trace log level, should be used for verbose debugging purposes.
	 */
    LOG_LEVEL_TRACE = 5,
} LogLevel;

b8 logging_system_create(LogLevel level);
void logging_system_destroy(void);

/**
 * @brief Outputs logging at the given level.
 * @param level The log level to use.
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
void _log_output(LogLevel level, const char* func, const char* message, ...);

/**
 * @brief Logs a fatal-level message. Should be used to stop the application
 * when hit.
 * @param message The message to be logged. Can be a format string for
 * additional parameters.
 * @param ... Additional parameters to be logged.
 */
#define FATAL(message, ...) _log_output(LOG_LEVEL_FATAL, __FUNCTION__, message, ##__VA_ARGS__);

#ifndef ERROR
/**
 * @brief Logs an error-level message. Should be used to indicate critical
 * runtime problems that cause the application to run improperly or not at all.
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define ERROR(message, ...) _log_output(LOG_LEVEL_ERROR, __FUNCTION__, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
/**
 * @brief Logs a warning-level message. Should be used to indicate non-critial
 * problems with the application that cause it to run suboptimally.
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define WARN(message, ...) _log_output(LOG_LEVEL_WARN, __FUNCTION__, message, ##__VA_ARGS__);
#else
/**
 * @brief Logs a warning-level message. Should be used to indicate non-critial
 * problems with the application that cause it to run suboptimally. Does nothing
 * when LOG_WARN_ENABLED != 1
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
/**
 * @brief Logs an info-level message. Should be used for non-erronuous
 * informational purposes.
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define INFO(message, ...) _log_output(LOG_LEVEL_INFO, __FUNCTION__, message, ##__VA_ARGS__);
#else
/**
 * @brief Logs an info-level message. Should be used for non-erronuous
 * informational purposes. Does nothing when LOG_INFO_ENABLED != 1
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
/**
 * @brief Logs a debug-level message. Should be used for debugging purposes.
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define DEBUG(message, ...) _log_output(LOG_LEVEL_DEBUG, __FUNCTION__, message, ##__VA_ARGS__);
#else
/**
 * @brief Logs a debug-level message. Should be used for debugging purposes.
 * Does nothing when LOG_DEBUG_ENABLED != 1
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
/**
 * @brief Logs a trace-level message. Should be used for verbose debugging
 * purposes.
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define TRACE(message, ...) _log_output(LOG_LEVEL_TRACE, __FUNCTION__, message, ##__VA_ARGS__);
#else
/**
 * @brief Logs a trace-level message. Should be used for verbose debugging
 * purposes. Does nothing when LOG_TRACE_ENABLED != 1
 * @param message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define TRACE(message, ...)
#endif
