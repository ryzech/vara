set(EDITOR_SOURCES src/editor_main.c
        src/ui/editor_ui.c
        src/ui/editor_panel.c)

add_executable(vara-editor ${EDITOR_SOURCES})

target_include_directories(vara-editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(vara-editor PRIVATE vara-core vara-runtime vara-opengl-renderer vara-vulkan-renderer)
