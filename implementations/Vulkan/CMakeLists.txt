set(AGPU_Vulkan_SOURCES
    buffer.cpp
    buffer.hpp
    command_allocator.cpp
    command_allocator.hpp
    command_list.cpp
    command_list.hpp
    command_queue.cpp
    command_queue.hpp
    common.hpp
    compute_pipeline_builder.hpp
    compute_pipeline_builder.cpp
    device.cpp
    device.hpp
    descriptor_pool.cpp
    descriptor_pool.hpp
    fence.cpp
    fence.hpp
    framebuffer.cpp
    framebuffer.hpp
    icd.cpp
    implicit_resource_command_list.cpp
    implicit_resource_command_list.hpp
    memory_allocator.cpp
    pipeline_builder.cpp
    pipeline_builder.hpp
    pipeline_state.cpp
    pipeline_state.hpp
    platform.cpp
    platform.hpp
    renderpass.cpp
    renderpass.hpp
    sampler.cpp
    sampler.hpp
    shader_resource_binding.cpp
    shader_resource_binding.hpp
    shader_signature.cpp
    shader_signature.hpp
    shader_signature_builder.cpp
    shader_signature_builder.hpp
    shader.cpp
    shader.hpp
    swap_chain.cpp
    swap_chain.hpp
    texture.cpp
    texture.hpp
    texture_format.hpp
    texture_view.cpp
    texture_view.hpp
    vertex_binding.cpp
    vertex_binding.hpp
    vertex_layout.cpp
    vertex_layout.hpp
    vk_mem_alloc.h
)

include_directories(${VULKAN_INCLUDE_DIRS})

add_definitions(
    -DAGPU_BUILD
    -DVMA_STATIC_VULKAN_FUNCTIONS=0 # Do not link with newer vulkan functions staticly. This causes some crashes.
)

if(BUILD_WITH_OPENVR)
    add_definitions(
        -DVR_API_EXPORT=1 # We are embedding the api implementation, so this has to match
        -DBUILD_WITH_OPENVR
    )
    set(AGPU_Vulkan_SOURCES ${AGPU_Vulkan_SOURCES}
        vr_system.cpp
        vr_system.hpp
    )
endif()

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(-DWIN64)
  endif()
endif()

if(BUILD_MULTIPLE_BACKENDS)
    set(AgpuVulkan AgpuVulkan)
else()
    set(AgpuVulkan Agpu)
endif()

add_library(${AgpuVulkan} SHARED ${AGPU_Vulkan_SOURCES})
add_dependencies(${AgpuVulkan} ${AgpuCommonHighLevelInterfaces_DEPS})

if(BUILD_WITH_OPENVR)
    add_dependencies(${AgpuVulkan} openvr_embeddedapi)
    include_directories(${OPENVR_INCLUDE})
    target_link_libraries(${AgpuVulkan}
        $<TARGET_OBJECTS:openvr_embeddedapi>)
endif()
target_link_libraries(${AgpuVulkan}
    ${VULKAN_LIBRARY} ${VULKAN_WSYS_LIBRARIES}
    ${AgpuCommonHighLevelInterfaces_LIBS})
