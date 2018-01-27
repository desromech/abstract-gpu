add_subdirectory(Loader)

set(LIBRARY_OUTPUT_PATH "${AGPU_BINARY_DIR}/dist/AgpuIcd")

if(D3D12_FOUND AND BUILD_D3D12)
    add_subdirectory(Direct3D12)
endif()

if(VULKAN_FOUND AND BUILD_VULKAN)
  add_subdirectory(Vulkan)
endif()

if(OPENGL_FOUND AND NOT APPLE AND BUILD_OPENGL)
    add_subdirectory(OpenGL)
endif()

if(METAL_FOUND AND BUILD_METAL)
  add_subdirectory(Metal)
endif()
