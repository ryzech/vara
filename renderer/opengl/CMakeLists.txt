set(OPENGL_RENDERER_SOURCES src/gl.c src/renderer_opengl_backend.c src/shader_opengl_backend.c src/shader_compiler_opengl_backend.c src/render_pass_opengl_backend.c src/buffer_opengl_backend.c
        src/framebuffer_opengl_backend.c
        src/texture_opengl_backend.c
        src/render_pipeline_opengl_backend.c
        src/swapchain_opengl_backend.c)

add_library(vara-opengl-renderer STATIC ${OPENGL_RENDERER_SOURCES})

target_include_directories(vara-opengl-renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-opengl-renderer PUBLIC vara-core vara-runtime)
