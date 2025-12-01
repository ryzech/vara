function(generate_shader_headers TARGET SHADER_DIR)
    file(GLOB SHADER_FILES "${SHADER_DIR}/*.glsl")
    set(GENERATED_HEADERS "")

    foreach (SHADER ${SHADER_FILES})
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
        set(OUTPUT_HEADER ${SHADER_DIR}/${SHADER_NAME}.glsl.gen.h)

        add_custom_command(
                OUTPUT ${OUTPUT_HEADER}
                COMMAND python3 ${CMAKE_SOURCE_DIR}/cmake/generate_shader_headers.py ${SHADER}
                DEPENDS ${SHADER} ${CMAKE_SOURCE_DIR}/cmake/generate_shader_headers.py
                COMMENT "Generating C header from ${SHADER_NAME}.glsl"
                VERBATIM
        )

        list(APPEND GENERATED_HEADERS ${OUTPUT_HEADER})
    endforeach ()

    # Create a custom target that depends on all generated headers
    add_custom_target(${TARGET}_shaders DEPENDS ${GENERATED_HEADERS})
    add_dependencies(${TARGET} ${TARGET}_shaders)
endfunction()