cmake_minimum_required(VERSION 3.20)

project(vara-engine LANGUAGES C CXX OBJC VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Has to be set to suppress duplicate library warnings.
# Not an issue, just an artifact of being explicit in the links.
cmake_policy(SET CMP0156 NEW)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_compile_definitions(VARA_VERSION="${PROJECT_VERSION}")

add_compile_options(
    -Wno-gnu-zero-variadic-macro-arguments 
    -Wno-pedantic
    -Wno-unused-variable  
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Wno-sign-compare
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(VARA_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(VARA_RELEASE)
endif()

set(EXTRA_LIBS "")

if(APPLE)
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
elseif(UNIX AND NOT APPLE)
  list(APPEND EXTRA_LIBS m rt)
elseif(WIN32)
  
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/deps.cmake)

add_subdirectory(core)
add_subdirectory(runtime)

add_subdirectory(renderer)

add_subdirectory(sandbox)
add_subdirectory(editor)
