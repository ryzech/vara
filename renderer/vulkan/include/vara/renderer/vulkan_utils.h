#pragma once

#include <vara/core/logger.h>

#define VK_CHECK(expr)                                                                             \
    {                                                                                              \
        VkResult result = expr;                                                                    \
        if (result == VK_SUCCESS) {                                                                \
        } else {                                                                                   \
            WARN("Vulkan call failed at line %d with code %d", __LINE__, result);                  \
        }                                                                                          \
    }
