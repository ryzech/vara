# Has to be set to suppress duplicate library warnings.
# Not an issue, just an artifact of being explicit in the links.
cmake_policy(SET CMP0156 NEW)

add_compile_options(-Wall -Wextra -Werror)
add_compile_options(
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-pedantic
        -Wno-unused-variable
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-sign-compare
        -Wno-visibility
        -Wno-missing-braces
        -Wno-unused-function
)

# Add debug symbols if in debug mode. (CMake should already, but to be explicit.)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g)
    add_link_options(-g)
endif ()

set(EXTRA_LIBS "")

if (APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(SECURITY_FRAMEWORK Security REQUIRED)
    list(APPEND EXTRA_LIBS
            ${FOUNDATION_FRAMEWORK}
            ${APPKIT_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${SECURITY_FRAMEWORK}
    )
elseif (UNIX AND NOT APPLE)
    list(APPEND EXTRA_LIBS m rt)
elseif (WIN32)

endif ()