#ifndef _AGPU_METAL_DEVICE_HPP_
#define _AGPU_METAL_DEVICE_HPP_

#include "implicit_resource_command_list.hpp"
#import <Metal/Metal.h>

#if ! __has_feature(objc_arc)
#error "ARC is off. This is mandatory for the AGPU Metal backend."
#endif

namespace AgpuMetal
{

class AMtlDevice : public agpu::device
{
public:
    AMtlDevice();
    ~AMtlDevice();

    static agpu::device_ref open(id<MTLDevice> selectedDevice, agpu_device_open_info *openInfo);

    virtual agpu::command_queue_ptr getDefaultCommandQueue() override;
    id<MTLCommandQueue> getDefaultCommandQueueHandle();

    virtual agpu::swap_chain_ptr createSwapChain(const agpu::command_queue_ref &commandQueue, agpu_swap_chain_create_info* swapChainInfo) override;
	virtual agpu::buffer_ptr createBuffer(agpu_buffer_description* description, agpu_pointer initial_data) override;
	virtual agpu::vertex_layout_ptr createVertexLayout() override;
	virtual agpu::vertex_binding_ptr createVertexBinding(const agpu::vertex_layout_ref &layout) override;
	virtual agpu::shader_ptr createShader(agpu_shader_type type) override;
	virtual agpu::shader_signature_builder_ptr createShaderSignatureBuilder() override;
	virtual agpu::pipeline_builder_ptr createPipelineBuilder() override;
	virtual agpu::compute_pipeline_builder_ptr createComputePipelineBuilder() override;
	virtual agpu::command_allocator_ptr createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref &queue) override;
	virtual agpu::command_list_ptr createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state) override;
	virtual agpu_shader_language getPreferredShaderLanguage() override;
	virtual agpu_shader_language getPreferredIntermediateShaderLanguage() override;
	virtual agpu_shader_language getPreferredHighLevelShaderLanguage() override;
	virtual agpu::framebuffer_ptr createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref *colorViews, const agpu::texture_view_ref &depthStencilView) override;
	virtual agpu::renderpass_ptr createRenderPass(agpu_renderpass_description* description) override;
	virtual agpu::texture_ptr createTexture(agpu_texture_description* description) override;
    virtual agpu::sampler_ptr createSampler(agpu_sampler_description* description) override;
	virtual agpu::fence_ptr createFence() override;
	virtual agpu_int getMultiSampleQualityLevels(agpu_texture_format format, agpu_uint sample_count) override;
	virtual agpu_bool hasTopLeftNdcOrigin() override;
	virtual agpu_bool hasBottomLeftTextureCoordinates() override;
	virtual agpu_cstring getName() override;
	virtual agpu_device_type getType() override;
	virtual agpu_bool isFeatureSupported(agpu_feature feature) override;
	virtual agpu_uint getLimitValue(agpu_limit limit) override;
	virtual agpu::vr_system_ptr getVRSystem() override;
	virtual agpu::window_scraper_ptr createWindowScraper() override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;
    virtual agpu::state_tracker_cache_ptr createStateTrackerCache(const agpu::command_queue_ref & command_queue_family) override;
    virtual agpu_error finishExecution() override;

    id<MTLDevice> device;

    template<typename FT>
    void withUploadCommandListDo(size_t requiredCpuBufferSize, size_t requiredCpuBufferAlignment, const FT &f)
    {
        std::unique_lock<std::mutex> l(implicitResourceUploadCommandList.mutex);
        implicitResourceUploadCommandList.ensureValidCPUStagingBuffer(requiredCpuBufferSize, requiredCpuBufferAlignment);

        f(implicitResourceUploadCommandList);
    }

    template<typename FT>
    void withReadbackCommandListDo(size_t requiredCpuBufferSize, size_t requiredCpuBufferAlignment, const FT &f)
    {
        std::unique_lock<std::mutex> l(implicitResourceReadbackCommandList.mutex);
        implicitResourceReadbackCommandList.ensureValidCPUStagingBuffer(requiredCpuBufferSize, requiredCpuBufferAlignment);

        f(implicitResourceReadbackCommandList);
    }

private:
    agpu::command_queue_ref mainCommandQueue;

    AMtlImplicitResourceUploadCommandList implicitResourceUploadCommandList;
    AMtlImplicitResourceReadbackCommandList implicitResourceReadbackCommandList;
};

} // End of namespace AgpuMetal

#endif //_AGPU_METAL_DEVICE_HPP_
