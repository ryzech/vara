#include <vara/core/logger.h>
#include "vara/application/application.h"

int main(int argc, char** argv) {
    int ret = application_main(argc, argv);
    INFO("Exited with code: %d", ret);
    return ret;
}
