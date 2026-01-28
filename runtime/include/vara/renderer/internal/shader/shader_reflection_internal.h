#pragma once

#include <vara/core/defines.h>

#include "vara/renderer/render_types.h"

typedef struct ReflectedShader ReflectedShader;
typedef struct ReflectedShaderStage ReflectedShaderStage;
typedef struct ReflectedDescriptor ReflectedDescriptor;
typedef struct ReflectedVertexAttribute ReflectedVertexAttribute;
typedef enum ReflectedDescriptorType ReflectedDescriptorType;

enum ReflectedDescriptorType {
    DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    DESCRIPTOR_TYPE_STORAGE_BUFFER,
    DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    DESCRIPTOR_TYPE_STORAGE_IMAGE,
};

struct ReflectedDescriptor {
    ReflectedDescriptorType type;
    char name[64];
    u32 set;
    u32 binding;
    u32 count;
    ShaderStage stage_mask;
};

struct ReflectedVertexAttribute {
    char name[64];
    u32 location;
    VertexAttributeType type;
};

struct ReflectedShaderStage {
    ShaderStage stage;
    u32 descriptor_count;
    ReflectedDescriptor descriptors[32];
    u32 vertex_attribute_count;
    ReflectedVertexAttribute vertex_attributes[32];
};

struct ReflectedShader {
    u32 descriptor_count;
    ReflectedDescriptor descriptors[32];
    u32 vertex_attribute_count;
    ReflectedVertexAttribute vertex_attributes[32];
};