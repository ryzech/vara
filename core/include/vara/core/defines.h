#pragma once

#include <stdint.h>

/** @brief Unsigned 8-bit integer */
typedef uint8_t u8;

/** @brief Unsigned 16-bit integer */
typedef uint16_t u16;

/** @brief Unsigned 32-bit integer */
typedef uint32_t u32;

/** @brief Unsigned 64-bit integer */
typedef uint64_t u64;

/** @brief Signed 8-bit integer */
typedef int8_t i8;

/** @brief Signed 16-bit integer */
typedef int16_t i16;

/** @brief Signed 32-bit integer */
typedef int32_t i32;

/** @brief Signed 64-bit integer */
typedef int64_t i64;

/** @brief 32-bit floating point number */
typedef float f32;

/** @brief 64-bit floating point number */
typedef double f64;

/** @brief 32-bit boolean type, used for APIs which require it */
typedef int32_t b32;

/** @brief 8-bit boolean type */
typedef uint8_t b8;

/** @brief A range, typically of memory */
typedef struct range {
    /** @brief The offset in bytes. */
    u64 offset;
    /** @brief The size in bytes. */
    u64 size;
} range;

/** @brief A range, typically of memory */
typedef struct range32 {
    /** @brief The offset in bytes. */
    i32 offset;
    /** @brief The size in bytes. */
    i32 size;
} range32;

/** @brief True.*/
#define true    1

/** @brief False. */
#define false   0

#define U64_MAX UINT64_MAX
#define U32_MAX UINT32_MAX
#define U16_MAX UINT16_MAX
#define U8_MAX  UINT8_MAX
#define U64_MIN UINT64_MIN
#define U32_MIN UINT32_MIN
#define U16_MIN UINT16_MIN
#define U8_MIN  UINT8_MIN

#define I8_MAX  INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX
#define I8_MIN  INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define VARA_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define VARA_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define VARA_PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define VARA_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define VARA_PLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define VARA_PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define VARA_PLATFORM_IOS           1
#define VARA_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define VARA_PLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif
