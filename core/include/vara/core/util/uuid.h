#pragma once

#include "vara/core/defines.h"

#define UUID_STRING_BYTES 37

typedef struct UUID {
    u8 bytes[16];
} UUID;

b8 uuid_generate(UUID* uuid);

void uuid_to_string(UUID* uuid, char* string);

b8 uuid_from_string(UUID* uuid, char* string);
