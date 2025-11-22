add_compile_definitions(VARA_VERSION="${PROJECT_VERSION}")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(VARA_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(VARA_RELEASE)
endif ()