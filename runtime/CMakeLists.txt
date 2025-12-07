set(RUNTIME_SOURCES src/application/application.c src/renderer/renderer_frontend.c src/renderer/shader_frontend.c src/renderer/render_pass_frontend.c src/renderer/buffer_frontend.c
        src/application/entrypoint.c
        src/camera/camera.c
        src/renderer/framebuffer_frontend.c
        src/renderer/render_command_frontend.c
        src/renderer2d/renderer2d.c
        src/renderer/texture_frontend.c
        src/vendor/stb_image.c
        src/renderer/internal/renderer_internal.c
        src/vendor/stb_truetype.c
        src/renderer/font.c
        src/scene/node.c
        src/ui/ui.c
)

add_library(vara-runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(vara-runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(vara-runtime PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(vara-runtime PRIVATE ${FREETYPE_LIBRARIES})
target_link_libraries(vara-runtime PUBLIC vara-core)

# Generate shader headers
include(${CMAKE_SOURCE_DIR}/cmake/generate_shader.cmake)
generate_shader_headers(vara-runtime ${CMAKE_CURRENT_SOURCE_DIR}/include/vara/shaders)