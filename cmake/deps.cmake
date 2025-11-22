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

find_package(shaderc 2025.4.1 QUIET)
if (NOT shaderc_FOUND)
    # Needed for libshaderc compilation!
    FetchContent_Declare(
            spirv-headers
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/KhronosGroup/SPIRV-Headers/archive/refs/tags/vulkan-sdk-1.4.328.1.zip
    )
    FetchContent_MakeAvailable(spirv-headers)
    # Needed for libshaderc compilation!
    FetchContent_Declare(
            spirv-tools
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/KhronosGroup/SPIRV-Tools/archive/refs/tags/v2025.4.zip
    )
    FetchContent_MakeAvailable(spirv-tools)
    # Needed for libshaderc compilation!
    FetchContent_Declare(
            glslang
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/KhronosGroup/glslang/archive/refs/tags/16.0.0.zip
    )
    set(GLSLANG_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glslang)

    FetchContent_Declare(
            shaderc
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/google/shaderc/archive/refs/tags/v2025.4.zip
    )
    set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
    set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
    set(SHADERC_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
    set(SHADERC_ENABLE_WERROR_COMPILE OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(shaderc)
endif ()
