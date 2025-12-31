function(generate_shader shader)
    get_filename_component(SHADER_NAME ${shader} NAME)
    string(REPLACE ".glsl" "" SHADER_NAME ${SHADER_NAME})
    set(OUTPUT_HEADER ${SHADER_DIR}/${SHADER_NAME}.glsl.gen.h)

    add_custom_command(
            OUTPUT ${OUTPUT_HEADER}
            COMMAND python3 ${CMAKE_SOURCE_DIR}/cmake/generate_shader_headers.py ${shader}
            COMMENT "Generating header for ${shader}"
            DEPENDS ${shader} ${SHADER_FILES}
            VERBATIM
    )
    list(APPEND GENERATED_SHADERS ${OUTPUT_HEADER})
    set(GENERATED_SHADERS ${GENERATED_SHADERS} PARENT_SCOPE)
endfunction()

function(generate_shader_headers TARGET SHADER_DIR)
    file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS ${SHADER_DIR}/*.glsl)
    foreach (SHADER ${SHADER_FILES})
        generate_shader(${SHADER})
    endforeach ()
    add_custom_target(${TARGET}_shaders ALL DEPENDS ${GENERATED_SHADERS})
    add_dependencies(${TARGET} ${TARGET}_shaders)
endfunction()