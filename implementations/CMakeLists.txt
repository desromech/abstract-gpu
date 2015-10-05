add_subdirectory(Loader)

set(LIBRARY_OUTPUT_PATH "${AGPU_BINARY_DIR}/dist/AgpuIcd")

if(DIRECT3D12_FOUND)
    add_subdirectory(Direct3D12)
endif()

if(OPENGL_FOUND)
    add_subdirectory(OpenGL)
endif()
