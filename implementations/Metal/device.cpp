#include "device.hpp"

// The exported C interface
AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo )
{
    return nullptr;
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data )
{
    return nullptr;
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding ( agpu_device* device, agpu_vertex_layout* layout )
{
    return nullptr;
}

AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type )
{
    return nullptr;
}

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue )
{
    return nullptr;
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    return nullptr;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage ( agpu_device* device )
{
    return AGPU_SHADER_LANGUAGE_METAL;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage ( agpu_device* device )
{
    return AGPU_SHADER_LANGUAGE_METAL;
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView )
{
    return nullptr;
}

AGPU_EXPORT agpu_texture* agpuCreateTexture ( agpu_device* device, agpu_texture_description* description )
{
    return nullptr;
}

AGPU_EXPORT agpu_fence* agpuCreateFence ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels ( agpu_device* device, agpu_uint sample_count )
{
    return sample_count == 1 ? 1 : 0;
}

AGPU_EXPORT agpu_bool agpuHasTopLeftNdcOrigin ( agpu_device* device )
{
    return AGPU_UNIMPLEMENTED;
}
