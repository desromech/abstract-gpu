set(AGPU_OpenGL_SOURCES
    buffer.cpp
    buffer.hpp
    command_allocator.cpp
    command_allocator.hpp
    command_list.cpp
    command_list.hpp
    command_queue.cpp
    command_queue.hpp
    compute_pipeline_builder.cpp
    compute_pipeline_builder.hpp
    common.hpp
    device.cpp
    device.hpp
    device_unix.cpp
    device_win32.cpp
    fence.cpp
    fence.hpp
    framebuffer.cpp
    framebuffer.hpp
    icd.cpp
    job_queue.hpp
    pipeline_builder.cpp
    pipeline_builder.hpp
    pipeline_state.cpp
    pipeline_state.hpp
    platform.cpp
    renderpass.cpp
    renderpass.hpp
    shader.cpp
    shader.hpp
    shader_resource_binding.cpp
    shader_resource_binding.hpp
    shader_signature.cpp
    shader_signature.hpp
    shader_signature_builder.cpp
    shader_signature_builder.hpp
    swap_chain.cpp
    swap_chain.hpp
    texture.cpp
    texture.hpp
    texture_formats.hpp
    vertex_binding.cpp
    vertex_binding.hpp
    vertex_layout.cpp
    vertex_layout.hpp
)

include_directories(${OPENGL_INCLUDE_DIR})

add_definitions(-DAGPU_BUILD)

add_library(AgpuOpenGL SHARED ${AGPU_OpenGL_SOURCES})
add_dependencies(AgpuOpenGL spirv-cross-core spirv-cross-glsl)
target_link_libraries(AgpuOpenGL
    ${OPENGL_gl_LIBRARY} $<TARGET_OBJECTS:spirv-cross-core> $<TARGET_OBJECTS:spirv-cross-glsl>
    ${AgpuCommonHighLevelInterfaces_LIBS})
