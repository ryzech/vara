#include <stdio.h>

#include "core/util/uuid.h"
#include "core/platform/platform.h"

b8 uuid_generate(UUID* uuid) {
    if (!platform_get_random_bytes(uuid->bytes, sizeof(uuid->bytes))) {
        return false;
    }

    /* Set version to 4 (random) */
    uuid->bytes[6] = (uuid->bytes[6] & 0x0F) | 0x40;

    /* Set variant to RFC 4122 */
    uuid->bytes[8] = (uuid->bytes[8] & 0x3F) | 0x80;

    return true;
}

void uuid_to_string(UUID* uuid, char* string) {
    snprintf(
        string,
        UUID_STRING_BYTES,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid->bytes[0],
        uuid->bytes[1],
        uuid->bytes[2],
        uuid->bytes[3],
        uuid->bytes[4],
        uuid->bytes[5],
        uuid->bytes[6],
        uuid->bytes[7],
        uuid->bytes[8],
        uuid->bytes[9],
        uuid->bytes[10],
        uuid->bytes[11],
        uuid->bytes[12],
        uuid->bytes[13],
        uuid->bytes[14],
        uuid->bytes[15]
    );
}
