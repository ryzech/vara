#include "core/entrypoint.h"
#include "core/logger.h"

int main(int argc, char** argv) {
    int ret = application_main(argc, argv);
    INFO("Exited with code: %d", ret);
    return ret;
}
