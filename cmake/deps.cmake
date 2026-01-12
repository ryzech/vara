include(FetchContent)

find_package(glfw3 3.4 QUIET)
if (NOT glfw3_FOUND)
    FetchContent_Declare(
            glfw3
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
    )
    FetchContent_MakeAvailable(glfw3)
endif ()

find_package(Freetype 2.14.1 QUIET)
if (NOT Freetype_FOUND)
    FetchContent_Declare(
            Freetype
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/freetype/freetype/archive/refs/tags/VER-2-14-1.zip
    )
    FetchContent_MakeAvailable(freetype)
endif ()

find_package(flecs 4.1.3 QUIET)
if (NOT flecs_FOUND)
    FetchContent_Declare(
            flecs
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/SanderMertens/flecs/archive/refs/tags/v4.1.3.zip
    )
    FetchContent_MakeAvailable(flecs)
endif ()

FetchContent_Declare(
        glslang
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/KhronosGroup/glslang/archive/refs/tags/16.1.0.zip
)
set(GLSLANG_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
set(GLSLANG_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glslang)

FetchContent_Declare(
        spirv-cross
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/vulkan-sdk-1.4.335.0.zip
)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_C_API ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spirv-cross)
