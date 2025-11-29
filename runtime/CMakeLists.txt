set(RUNTIME_SOURCES src/application/application.c src/renderer/renderer_frontend.c src/renderer/shader_frontend.c src/renderer/render_pass_frontend.c src/renderer/buffer_frontend.c
        src/application/entrypoint.c
        src/camera/camera.c
        src/renderer/framebuffer_frontend.c
        src/renderer/render_command.c
        src/renderer2d/renderer2d.c
        src/renderer/texture_frontend.c
        src/vendor/stb_image.c
)

add_library(vara-runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(vara-runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-runtime PUBLIC vara-core)
