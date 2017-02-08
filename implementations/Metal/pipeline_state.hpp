#ifndef AGPU_METAL_PIPELINE_STATE_HPP
#define AGPU_METAL_PIPELINE_STATE_HPP

#include "device.hpp"
#include "pipeline_command_state.hpp"

struct _agpu_pipeline_state : public Object<_agpu_pipeline_state>
{
public:
    _agpu_pipeline_state(agpu_device *device);
    void lostReferences();

    static agpu_pipeline_state *create(agpu_device *device, agpu_pipeline_builder *builder, id<MTLRenderPipelineState> handle);

    void applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder);

    agpu_device *device;
    id<MTLRenderPipelineState> handle;
    id<MTLDepthStencilState> depthStencilState;
    agpu_pipeline_command_state commandState;
};

#endif //AGPU_METAL_PIPELINE_STATE_HPP
