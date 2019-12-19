#include "device.hpp"
#include "swap_chain.hpp"
#include "shader_signature_builder.hpp"
#include "command_queue.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "fence.hpp"
#include "shader.hpp"
#include "pipeline_builder.hpp"
#include "compute_pipeline_builder.hpp"
#include "buffer.hpp"
#include "vertex_layout.hpp"
#include "vertex_binding.hpp"
#include "texture.hpp"
#include "sampler.hpp"
#include "../Common/offline_shader_compiler.hpp"
#include "../Common/state_tracker_cache.hpp"

namespace AgpuMetal
{

AMtlDevice::AMtlDevice()
    :
    implicitResourceUploadCommandList(*this),
    implicitResourceReadbackCommandList(*this)
{
}

AMtlDevice::~AMtlDevice()
{
    implicitResourceUploadCommandList.destroy();
    implicitResourceReadbackCommandList.destroy();

    mainCommandQueue.reset();
    if(device)
        [device release];
}

agpu::device_ref AMtlDevice::open(agpu_device_open_info *openInfo)
{
    id<MTLDevice> mtlDevice = MTLCreateSystemDefaultDevice();
    if(!mtlDevice)
        return agpu::device_ref();

    id<MTLCommandQueue> mainCommandQueue = [mtlDevice newCommandQueue];
    if(!mainCommandQueue)
    {
        [mtlDevice release];
        return agpu::device_ref();
    }

    auto result = agpu::makeObject<AMtlDevice> ();
    auto device = result.as<AMtlDevice> ();
    device->device = mtlDevice;
    device->mainCommandQueue = AMtlCommandQueue::create(result, mainCommandQueue);
    
    // Store a copy to in the implicit resource command lists.
    device->implicitResourceUploadCommandList.commandQueue = device->mainCommandQueue;
    device->implicitResourceReadbackCommandList.commandQueue = device->mainCommandQueue;

    return result;
}

agpu_bool AMtlDevice::isFeatureSupported(agpu_feature feature)
{
    switch(feature)
    {
    case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING:
    case AGPU_FEATURE_COHERENT_MEMORY_MAPPING:
    case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING:
        return true;

    case AGPU_FEATURE_COMMAND_LIST_REUSE:
    case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE:
        return false;

    default:
        return false;
    }
}

agpu::command_queue_ptr AMtlDevice::getDefaultCommandQueue()
{
    return mainCommandQueue.disownedNewRef();
}

id<MTLCommandQueue> AMtlDevice::getDefaultCommandQueueHandle()
{
    return mainCommandQueue.as<AMtlCommandQueue> ()->handle;
}

agpu::swap_chain_ptr AMtlDevice::createSwapChain(const agpu::command_queue_ref &commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
    return AMtlSwapChain::create(refFromThis<agpu::device> (), commandQueue, swapChainInfo).disown();
}

agpu::buffer_ptr AMtlDevice::createBuffer(agpu_buffer_description* description, agpu_pointer initial_data)
{
    return AMtlBuffer::create(refFromThis<agpu::device> (), description, initial_data).disown();
}

agpu::vertex_layout_ptr AMtlDevice::createVertexLayout()
{
    return AMtlVertexLayout::create(refFromThis<agpu::device> ()).disown();
}

agpu::vertex_binding_ptr AMtlDevice::createVertexBinding(const agpu::vertex_layout_ref &layout)
{
    return AMtlVertexBinding::create(refFromThis<agpu::device> (), layout).disown();
}

agpu::shader_ptr AMtlDevice::createShader(agpu_shader_type type)
{
    return AMtlShader::create(refFromThis<agpu::device> (), type).disown();
}

agpu::shader_signature_builder_ptr AMtlDevice::createShaderSignatureBuilder()
{
    return AMtlShaderSignatureBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::pipeline_builder_ptr AMtlDevice::createPipelineBuilder()
{
    return AMtlGraphicsPipelineBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::compute_pipeline_builder_ptr AMtlDevice::createComputePipelineBuilder()
{
    return AMtlComputePipelineBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::command_allocator_ptr AMtlDevice::createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref &queue)
{
    return AMtlCommandAllocator::create(refFromThis<agpu::device> (), type, queue).disown();
}

agpu::command_list_ptr AMtlDevice::createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    return AMtlCommandList::create(refFromThis<agpu::device> (), type, allocator, initial_pipeline_state).disown();
}

agpu_shader_language AMtlDevice::getPreferredShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_METAL_AIR;
}

agpu_shader_language AMtlDevice::getPreferredIntermediateShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
    //return AGPU_SHADER_LANGUAGE_NONE;
}

agpu_shader_language AMtlDevice::getPreferredHighLevelShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_METAL;
}

agpu::framebuffer_ptr AMtlDevice::createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref *colorViews, const agpu::texture_view_ref &depthStencilView)
{
    return AMtlFramebuffer::create(refFromThis<agpu::device> (), width, height, colorCount, colorViews, depthStencilView).disown();
}

agpu::renderpass_ptr AMtlDevice::createRenderPass(agpu_renderpass_description* description)
{
    return AMtlRenderPass::create(refFromThis<agpu::device> (), description).disown();
}

agpu::texture_ptr AMtlDevice::createTexture(agpu_texture_description* description)
{
    return AMtlTexture::create(refFromThis<agpu::device> (), description).disown();
}

agpu::sampler_ptr AMtlDevice::createSampler(agpu_sampler_description* description)
{
    return AMtlSampler::create(refFromThis<agpu::device> (), description).disown();
}

agpu::fence_ptr AMtlDevice::createFence()
{
    return AMtlFence::create(refFromThis<agpu::device> ()).disown();
}

agpu_int AMtlDevice::getMultiSampleQualityLevels(agpu_texture_format format, agpu_uint sample_count)
{
    return 1;
}

agpu_bool AMtlDevice::hasTopLeftNdcOrigin()
{
    return false;
}

agpu_bool AMtlDevice::hasBottomLeftTextureCoordinates()
{
    return false;
}

agpu::vr_system_ptr AMtlDevice::getVRSystem()
{
    return nullptr;
}

agpu::offline_shader_compiler_ptr AMtlDevice::createOfflineShaderCompiler()
{
	return AgpuCommon::GLSLangOfflineShaderCompiler::createForDevice(refFromThis<agpu::device> ()).disown();
}

agpu::state_tracker_cache_ptr AMtlDevice::createStateTrackerCache(const agpu::command_queue_ref & command_queue_family)
{
	return AgpuCommon::StateTrackerCache::create(refFromThis<agpu::device> (), 0).disown();
}

agpu_error AMtlDevice::finishExecution()
{
    return mainCommandQueue->finishExecution();
}

} // End of namespace AgpuMetal
