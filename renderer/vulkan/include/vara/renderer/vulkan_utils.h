#pragma once

#include <vara/core/logger.h>

#define VK_CHECK(expr)                                                                             \
    {                                                                                              \
        if (expr == VK_SUCCESS) {                                                                  \
        } else {                                                                                   \
            WARN("Vulkan call failed at (%s:%s)", __FILE__, __LINE__);                             \
        }                                                                                          \
    }