set(VULKAN_RENDERER_SOURCES src/volk/volk.c src/renderer_vulkan_backend.c
        src/vulkan_device.c
        src/vulkan_platform.c
        src/swapchain_vulkan_backend.c
        src/render_pass_vulkan_backend.c
        src/shader_vulkan_backend.c
        src/render_pipeline_vulkan_backend.c
        src/vma/vk_mem_alloc.cpp
        src/buffer_vulkan_backend.c
        src/texture_vulkan_backend.c
        src/render_target_vulkan_backend.c
)

add_library(vara-vulkan-renderer STATIC ${VULKAN_RENDERER_SOURCES})

target_include_directories(vara-vulkan-renderer PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(vara-vulkan-renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-vulkan-renderer PUBLIC vara-core vara-runtime)
