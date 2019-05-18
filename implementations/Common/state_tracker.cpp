#include "state_tracker.hpp"

namespace AgpuCommon
{

AbstractStateTracker::AbstractStateTracker(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue)
    : device(device), cache(cache), commandListType(type), commandQueue(commandQueue)
{
}

AbstractStateTracker::~AbstractStateTracker()
{
}

agpu_error AbstractStateTracker::reset()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::resetGraphicsPipeline()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::resetComputePipeline()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setComputeStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setVertexStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setFragmentStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setFrontFace(agpu_face_winding winding)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setCullMode(agpu_cull_mode mode)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setPolygonMode(agpu_polygon_mode mode)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setRenderTargetCount(agpu_int count)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setDepthStencilFormat(agpu_texture_format format)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setPrimitiveType(agpu_primitive_topology type)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setVertexLayout(const agpu::vertex_layout_ref & layout)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setShaderSignature(const agpu::shader_signature_ref & signature)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useVertexBinding(const agpu::vertex_binding_ref & vertex_binding)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useIndexBuffer(const agpu::buffer_ref & index_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useIndexBufferAt(const agpu::buffer_ref & index_buffer, agpu_size offset, agpu_size index_size)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useDrawIndirectBuffer(const agpu::buffer_ref & draw_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useComputeDispatchIndirectBuffer(const agpu::buffer_ref & buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useShaderResources(const agpu::shader_resource_binding_ref & binding)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::useComputeShaderResources(const agpu::shader_resource_binding_ref & binding)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::drawArraysIndirect(agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::drawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::dispatchComputeIndirect(agpu_size offset)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::setStencilReference(agpu_uint reference)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::executeBundle(const agpu::command_list_ref & bundle)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::beginRenderPass(const agpu::renderpass_ref & renderpass, const agpu::framebuffer_ref & framebuffer, agpu_bool bundle_content)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::endRenderPass()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::resolveFramebuffer(const agpu::framebuffer_ref & destFramebuffer, const agpu::framebuffer_ref & sourceFramebuffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::resolveTexture(const agpu::texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AbstractStateTracker::pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values)
{
    return AGPU_UNIMPLEMENTED;
}

//==============================================================================
// DirectStateTracker
//==============================================================================

DirectStateTracker::DirectStateTracker(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        const agpu::command_allocator_ref &commandAllocator,
        bool isCommandAllocatorOwned)
    : AbstractStateTracker(cache, device, type, commandQueue),
      commandAllocator(commandAllocator),
      isCommandAllocatorOwned(isCommandAllocatorOwned)
{
}

DirectStateTracker::~DirectStateTracker()
{
}

agpu::state_tracker_ref DirectStateTracker::create(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        const agpu::command_allocator_ref &commandAllocator,
        bool isCommandAllocatorOwned)
{
    return agpu::makeObject<DirectStateTracker> (cache, device, type, commandQueue, commandAllocator, isCommandAllocatorOwned);
}


//==============================================================================
// FrameBufferredStateTracker
//==============================================================================

FrameBufferredStateTracker::FrameBufferredStateTracker(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        agpu_uint frameBufferingCount)
    : AbstractStateTracker(cache, device, type, commandQueue),
      frameBufferingCount(frameBufferingCount)
{
}

FrameBufferredStateTracker::~FrameBufferredStateTracker()
{
}

agpu::state_tracker_ref FrameBufferredStateTracker::create(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        agpu_uint frameBufferingCount)
{
    auto result = agpu::makeObject<FrameBufferredStateTracker> (cache, device, type, commandQueue, frameBufferingCount);
    if(!result.as<FrameBufferredStateTracker> ()->createCommandAllocatorsAndCommandLists())
        return agpu::state_tracker_ref();
    return result;
}

bool FrameBufferredStateTracker::createCommandAllocatorsAndCommandLists()
{
    commandAllocators.reserve(frameBufferingCount);
    commandLists.reserve(frameBufferingCount);
    for(size_t i = 0; i < frameBufferingCount; ++i)
    {
        auto allocator = agpu::command_allocator_ref(device->createCommandAllocator(commandListType, commandQueue));
        if(!allocator)
            return false;

        auto commandList = agpu::command_list_ref(device->createCommandList(commandListType, allocator, agpu::pipeline_state_ref()));
        if(!commandList)
            return false;

        auto error = commandList->close();
        if(error)
            return false;

        commandAllocators.push_back(allocator);
        commandLists.push_back(commandList);
    }

    return true;
}

} // End of namespace AgpuCommon
