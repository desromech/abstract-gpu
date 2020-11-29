
# Add GLSLang
set(SKIP_GLSLANG_INSTALL YES)
add_subdirectory(glslang)

# Add Spir-V cross
add_subdirectory(spirv-cross)

# Add openvr
if(BUILD_WITH_OPENVR)
    add_subdirectory(openvr)
endif()
