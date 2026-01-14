set(VULKAN_RENDERER_SOURCES src/volk.c src/renderer_vulkan_backend.c
)

add_library(vara-vulkan-renderer STATIC ${VULKAN_RENDERER_SOURCES})

target_include_directories(vara-vulkan-renderer PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(vara-vulkan-renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-vulkan-renderer PUBLIC vara-core vara-runtime)
