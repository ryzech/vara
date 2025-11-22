cmake_minimum_required(VERSION 3.20)

project(vara-engine LANGUAGES C CXX VERSION 0.1.0)
if (APPLE)
    enable_language(OBJC)
endif ()

# Dependency include before compiler options are set.
include(cmake/deps.cmake)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Compiler options
# Includes platform specific libraries (ie. Foundation on macOS)
# And policies.
include(cmake/options.cmake)

# Compiler definitions
# Includes VARA_VERSION, VARA_RELEASE etc.
include(cmake/definitions.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(core)
add_subdirectory(runtime)
add_subdirectory(renderer)
add_subdirectory(sandbox)
add_subdirectory(editor)
