#include "device.hpp"
#include "swap_chain.hpp"
#include "shader_signature_builder.hpp"
#include "command_queue.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"

_agpu_device::_agpu_device()
{
    mainCommandQueue = nullptr;
}

void _agpu_device::lostReferences()
{
    if(mainCommandQueue)
    {
        mainCommandQueue->release();
        mainCommandQueue = nullptr;
    }
}

agpu_device *_agpu_device::open(agpu_device_open_info *openInfo)
{
    id<MTLDevice> mtlDevice = MTLCreateSystemDefaultDevice();
    if(!mtlDevice)
        return nullptr;

    id<MTLCommandQueue> mainCommandQueue = [mtlDevice newCommandQueue];
    if(!mainCommandQueue)
        return nullptr;

    auto result = new agpu_device();
    result->device = mtlDevice;
    result->mainCommandQueue = agpu_command_queue::create(result, mainCommandQueue);
    return result;
}

agpu_command_queue* _agpu_device::getDefaultCommandQueue()
{
    mainCommandQueue->retain();
    return mainCommandQueue;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device )
{
    CHECK_POINTER(device);
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device )
{
    CHECK_POINTER(device);
    return device->release();
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue ( agpu_device* device )
{
    if(!device)
        return nullptr;
    return device->getDefaultCommandQueue();
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo )
{
    if(!device)
        return nullptr;
    return agpu_swap_chain::create(device, swapChainInfo);
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
    if(!device)
        return nullptr;
    return agpu_shader_signature_builder::create(device);
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device )
{
    return nullptr;
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue )
{
    if(!device)
        return nullptr;
    return agpu_command_allocator::create(device, type, queue);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    if(!device)
        return nullptr;
    return agpu_command_list::create(device, type, allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage ( agpu_device* device )
{
    return AGPU_SHADER_LANGUAGE_METAL_AIR;
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
    return false;
}
