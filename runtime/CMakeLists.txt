set(RUNTIME_SOURCES src/application.c src/renderer_frontend.c src/shader_frontend.c src/render_pass_frontend.c src/buffer_frontend.c
        src/entrypoint.c
        include/vara/camera/camera.h)

add_library(vara-runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(vara-runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vara-runtime PUBLIC vara-core)
