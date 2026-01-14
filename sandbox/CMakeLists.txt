set(SANDBOX_SOURCES src/main.c)

add_executable(vara-sandbox ${SANDBOX_SOURCES})

target_link_libraries(vara-sandbox PRIVATE vara-core vara-runtime vara-opengl-renderer vara-vulkan-renderer)
