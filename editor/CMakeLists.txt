set(EDITOR_SOURCES src/editor_main.c)

add_executable(vara-editor ${EDITOR_SOURCES})

target_link_libraries(vara-editor PRIVATE vara-core vara-runtime vara-opengl-renderer)
