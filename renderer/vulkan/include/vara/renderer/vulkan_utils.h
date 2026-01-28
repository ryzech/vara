#pragma once

#include <vara/core/logger.h>

static VkFormat vertex_attribute_type_to_vk(VertexAttributeType type) {
    switch (type) {
        case VERTEX_ATTRIBUTE_FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case VERTEX_ATTRIBUTE_FLOAT2:
            return VK_FORMAT_R32G32_SFLOAT;
        case VERTEX_ATTRIBUTE_FLOAT3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case VERTEX_ATTRIBUTE_FLOAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case VERTEX_ATTRIBUTE_INT:
            return VK_FORMAT_R32_SINT;
        case VERTEX_ATTRIBUTE_INT2:
            return VK_FORMAT_R32G32_SINT;
        case VERTEX_ATTRIBUTE_INT3:
            return VK_FORMAT_R32G32B32_SINT;
        case VERTEX_ATTRIBUTE_INT4:
            return VK_FORMAT_R32G32B32A32_SINT;
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

static u32 vertex_attribute_type_size(VertexAttributeType type) {
    switch (type) {
        case VERTEX_ATTRIBUTE_FLOAT:
            return 4;
        case VERTEX_ATTRIBUTE_FLOAT2:
            return 8;
        case VERTEX_ATTRIBUTE_FLOAT3:
            return 12;
        case VERTEX_ATTRIBUTE_FLOAT4:
            return 16;
        case VERTEX_ATTRIBUTE_INT:
            return 4;
        case VERTEX_ATTRIBUTE_INT2:
            return 8;
        case VERTEX_ATTRIBUTE_INT3:
            return 12;
        case VERTEX_ATTRIBUTE_INT4:
            return 16;
        default:
            return 4;
    }
}

#define VK_CHECK(expr)                                                                             \
    {                                                                                              \
        VkResult result = expr;                                                                    \
        if (result == VK_SUCCESS) {                                                                \
        } else {                                                                                   \
            WARN("Vulkan call failed at line %d with code %d", __LINE__, result);                  \
        }                                                                                          \
    }
