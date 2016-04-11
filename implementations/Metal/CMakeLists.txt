set(AGPU_Metal_SOURCES
    command_allocator.hpp
    command_allocator.mm
    command_list.hpp
    command_list.mm
    command_queue.hpp
    command_queue.mm
    common.hpp
    device.hpp
    device.mm
    fence.hpp
    fence.mm
    framebuffer.hpp
    framebuffer.mm
    icd.c
    object.hpp
    platform.mm
    platform.hpp
    renderpass.hpp
    renderpass.mm
    stubs.mm
    shader_signature_builder.hpp
    shader_signature_builder.mm
    shader_signature.hpp
    shader_signature.mm
    swap_chain.hpp
    swap_chain.mm
    texture.hpp
    texture.mm
)

add_definitions(-DAGPU_BUILD)

add_library(AgpuMetal SHARED ${AGPU_Metal_SOURCES})
target_link_libraries(AgpuMetal ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY} ${APPKIT_LIBRARY})
