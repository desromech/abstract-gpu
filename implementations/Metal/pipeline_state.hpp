#ifndef AGPU_METAL_PIPELINE_STATE_HPP
#define AGPU_METAL_PIPELINE_STATE_HPP

#include "device.hpp"
#include "pipeline_command_state.hpp"

namespace AgpuMetal
{
    
class AMtlComputePipelineBuilder;
class AMtlGraphicsPipelineBuilder;

class AGPUMTLPipelineStateExtra
{
public:
    virtual ~AGPUMTLPipelineStateExtra() {}
    virtual void applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder) {}
    virtual void applyComputeCommands(id<MTLComputeCommandEncoder> computeEncoder) {}
    
    virtual bool isRender() const
    {
        return false;
    }
    
    virtual bool isCompute() const
    {
        return false;
    }
    
    virtual MTLSize getLocalSize()
    {
        return MTLSize();
    }
    
    virtual agpu_pipeline_command_state *getCommandState()
    {
        return nullptr;
    }
};

class AGPUMTLRenderPipelineState : public AGPUMTLPipelineStateExtra
{
public:
    AGPUMTLRenderPipelineState();
    ~AGPUMTLRenderPipelineState();
    
    virtual void applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder) override;

    virtual bool isRender() const override
    {
        return true;
    }
    
    virtual agpu_pipeline_command_state *getCommandState() override
    {
        return &commandState;
    }

    id<MTLRenderPipelineState> handle;
    id<MTLDepthStencilState> depthStencilState;
    agpu_pipeline_command_state commandState;
    
    agpu_float depthBiasConstantFactor;
    agpu_float depthBiasClamp;
    agpu_float depthBiasSlopeFactor;
};

class AGPUMTLComputePipelineState : public AGPUMTLPipelineStateExtra
{
public:
    AGPUMTLComputePipelineState();
    ~AGPUMTLComputePipelineState();
    
    virtual void applyComputeCommands(id<MTLComputeCommandEncoder> renderEncoder) override;
    
    virtual MTLSize getLocalSize() override
    {
        return localSize;
    }
    
    virtual bool isCompute() const override
    {
        return true;
    }

    id<MTLComputePipelineState> handle;
    MTLSize localSize;
};

struct AMtlPipelineState : public agpu::pipeline_state
{
public:
    AMtlPipelineState(const agpu::device_ref &device);
    ~AMtlPipelineState();

    static agpu::pipeline_state_ref createRender(const agpu::device_ref &device, AMtlGraphicsPipelineBuilder *builder, id<MTLRenderPipelineState> handle);
    static agpu::pipeline_state_ref createCompute(const agpu::device_ref &device, AMtlComputePipelineBuilder *builder, id<MTLComputePipelineState> handle);

    void applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder);
    void applyComputeCommands(id<MTLComputeCommandEncoder> renderEncoder);
    
    agpu_pipeline_command_state &getCommandState()
    {
        return *extraState->getCommandState();
    }

    agpu::device_ref device;
    std::unique_ptr<AGPUMTLPipelineStateExtra> extraState;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_PIPELINE_STATE_HPP
