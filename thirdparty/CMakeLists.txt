
# Add GLSLang
set(SKIP_GLSLANG_INSTALL YES)
add_subdirectory(glslang)

# Add Spir-V cross
function(add_spirv_cross_libraries)
	set(SPIRV_CROSS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spirv-cross)

    add_library(spirv-cross-core OBJECT
		${SPIRV_CROSS_SOURCE_DIR}/GLSL.std.450.h
		${SPIRV_CROSS_SOURCE_DIR}/spirv_common.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cross_containers.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cross_error_handling.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cross.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cross.cpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_parser.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_parser.cpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cross_parsed_ir.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cross_parsed_ir.cpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cfg.hpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_cfg.cpp
	)

	add_library(spirv-cross-msl OBJECT
		${SPIRV_CROSS_SOURCE_DIR}/spirv_msl.cpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_msl.hpp)

    add_library(spirv-cross-hlsl OBJECT
		${SPIRV_CROSS_SOURCE_DIR}/spirv_hlsl.cpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_hlsl.hpp)

    add_library(spirv-cross-glsl OBJECT
		${SPIRV_CROSS_SOURCE_DIR}/spirv_glsl.cpp
		${SPIRV_CROSS_SOURCE_DIR}/spirv_glsl.hpp)
endfunction()
add_spirv_cross_libraries()

# Add openvr
if(BUILD_WITH_OPENVR)
    add_subdirectory(openvr)
endif()
