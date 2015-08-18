set(AGPU_OpenGL_SOURCES
    buffer.cpp
    buffer.hpp
    command_allocator.cpp
    command_allocator.hpp
    command_list.cpp
    command_list.hpp
    command_queue.cpp
    command_queue.hpp
    common.hpp
    device.cpp
    device.hpp
    device_unix.cpp
    device_win32.cpp
    framebuffer.cpp
    framebuffer.hpp
    icd.c
    object.hpp
    pipeline_builder.cpp
    pipeline_builder.hpp
    pipeline_state.cpp
    pipeline_state.hpp
    platform.cpp
    platform.hpp
    shader.cpp
    shader.hpp
    shader_resource_binding.cpp
    shader_resource_binding.hpp
    vertex_binding.cpp
    vertex_binding.hpp
    vertex_layout.cpp
    vertex_layout.hpp
)

include_directories(${OPENGL_INCLUDE_DIR})

add_definitions(-DAGPU_BUILD)

add_library(AgpuOpenGL SHARED ${AGPU_OpenGL_SOURCES})
target_link_libraries(AgpuOpenGL ${OPENGL_gl_LIBRARY})
