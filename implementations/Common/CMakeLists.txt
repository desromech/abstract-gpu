add_definitions(-DAGPU_BUILD)

set(AgpuCommonHighLevelInterfaces_SOURCES
    offline_shader_compiler.cpp
    offline_shader_compiler.hpp
    state_tracker_cache.cpp
    state_tracker_cache.hpp
    state_tracker.cpp
    state_tracker.hpp
    immediate_renderer.cpp
    immediate_renderer.hpp
    memory_profiler.cpp
    memory_profiler.hpp
    overlay_window.hpp
    overlay_window.cpp
    overlay_window_win32.cpp

    window_scraper.cpp
    window_scraper_win32.cpp
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
