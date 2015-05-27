set(AGPU_OpenGL_SOURCES
    buffer.cpp
    context.cpp
    device.cpp
    device_unix.cpp
    icd.c
    platform.cpp
)

include_directories(${OPENGL_INCLUDE_DIR})

add_library(AgpuOpenGL SHARED ${AGPU_OpenGL_SOURCES})
target_link_libraries(AgpuOpenGL ${OPENGL_gl_LIBRARY})


