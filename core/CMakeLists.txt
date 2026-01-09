set(CORE_SOURCES src/logger.c src/util/uuid.c
        src/input/input.c
        src/event/event.c
        src/util/string.c
        src/math/geometry.c
        src/memory/arena_allocator.c)

if(APPLE)
    list(APPEND CORE_SOURCES
        src/platform/platform_darwin.m
        src/platform/platform_window_glfw.c
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND CORE_SOURCES
        src/platform/platform_linux.c
        src/platform/platform_window_glfw.c
    )
elseif(WIN32)
    list(APPEND CORE_SOURCES
        src/platform/platform_win32.c
        src/platform/platform_window_glfw.c
    )
endif()

add_library(vara-core STATIC ${CORE_SOURCES})

target_include_directories(vara-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-core PUBLIC glfw ${EXTRA_LIBS})
