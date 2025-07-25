#include "core/logger.h"
#include <stdlib.h>

int main(void) {
    TRACE("hi%s", "hello");
    return EXIT_SUCCESS;
}
