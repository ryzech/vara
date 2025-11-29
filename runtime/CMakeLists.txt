set(RUNTIME_SOURCES src/application.c src/renderer_frontend.c src/shader_frontend.c src/render_pass_frontend.c src/buffer_frontend.c
        src/entrypoint.c
        src/camera.c
        src/framebuffer_frontend.c
        src/render_command.c
        src/renderer2d.c
        src/texture_frontend.c
)

add_library(vara-runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(vara-runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-runtime PUBLIC vara-core)
