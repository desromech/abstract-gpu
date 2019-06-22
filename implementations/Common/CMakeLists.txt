add_definitions(-DAGPU_BUILD)

include_directories(
../../thirdparty/glslang
)

set(AgpuCommonHighLevelInterfaces_SOURCES
    offline_shader_compiler.cpp
    offline_shader_compiler.hpp
    state_tracker_cache.cpp
    state_tracker_cache.hpp
    state_tracker.cpp
    state_tracker.hpp
    immediate_renderer.cpp
    immediate_renderer.hpp
)

add_library(AgpuCommonHighLevelInterfaces OBJECT ${AgpuCommonHighLevelInterfaces_SOURCES})
add_dependencies(AgpuCommonHighLevelInterfaces glslang)
set_property(TARGET AgpuCommonHighLevelInterfaces PROPERTY POSITION_INDEPENDENT_CODE ON)
set(AgpuCommonHighLevelInterfaces_LIBS
    $<TARGET_OBJECTS:AgpuCommonHighLevelInterfaces>
    $<TARGET_OBJECTS:OSDependent>
    $<TARGET_OBJECTS:OGLCompiler>
    $<TARGET_OBJECTS:HLSL>
    $<TARGET_OBJECTS:glslang>
    $<TARGET_OBJECTS:glslang-default-resource-limits>
    $<TARGET_OBJECTS:SPIRV>
PARENT_SCOPE)

set(AgpuCommonHighLevelInterfaces_DEPS
    AgpuCommonHighLevelInterfaces
    OSDependent
    OGLCompiler
    HLSL
    glslang
    glslang-default-resource-limits
    SPIRV
PARENT_SCOPE)
