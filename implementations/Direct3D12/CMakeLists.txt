set(AGPU_Direct3D12_SOURCES
    buffer.cpp
    buffer.hpp
    command_allocator.cpp
    command_allocator.hpp
    command_list.cpp
    command_list.hpp
    command_queue.cpp
    command_queue.hpp
    common.hpp
	common_commands.hpp
	compute_pipeline_builder.cpp
	compute_pipeline_builder.hpp
    D3D12MemAlloc.cpp
    D3D12MemAlloc.h
    device.cpp
    device.hpp
    fence.cpp
    fence.hpp
    framebuffer.cpp
    framebuffer.hpp
    icd.cpp
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
    texture_view.cpp
    texture_view.hpp
    texture.cpp
    texture.hpp
    texture_formats.hpp
    vertex_binding.cpp
    vertex_binding.hpp
    vertex_layout.cpp
    vertex_layout.hpp
)

include_directories(${D3D12_INCLUDE_DIRS})

add_definitions(
    -DAGPU_BUILD
    -DVR_API_EXPORT=1 # We are embedding the api implementation, so this has to match
)

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions( -DWIN64 )
  endif()
endif()

add_library(AgpuDirect3D12 SHARED ${AGPU_Direct3D12_SOURCES})
add_dependencies(AgpuDirect3D12
	spirv-cross-core spirv-cross-glsl spirv-cross-hlsl
	openvr_embeddedapi
	${AgpuCommonHighLevelInterfaces_DEPS})
include_directories(${OPENVR_INCLUDE})
target_link_libraries(AgpuDirect3D12
    ${D3D12_LIBRARIES}
	$<TARGET_OBJECTS:spirv-cross-core> $<TARGET_OBJECTS:spirv-cross-hlsl> $<TARGET_OBJECTS:spirv-cross-glsl>
    $<TARGET_OBJECTS:openvr_embeddedapi> ${OPENVR_LIBS}
    ${AgpuCommonHighLevelInterfaces_LIBS})
