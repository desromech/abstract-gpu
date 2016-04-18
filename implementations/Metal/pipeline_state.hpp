#ifndef AGPU_METAL_PIPELINE_STATE_HPP
#define AGPU_METAL_PIPELINE_STATE_HPP

#include "device.hpp"

struct _agpu_pipeline_state : public Object<_agpu_pipeline_state>
{
public:
    _agpu_pipeline_state(agpu_device *device);
    void lostReferences();

    static agpu_pipeline_state *create(agpu_device *device, id<MTLRenderPipelineState> handle);
    
    agpu_device *device;
    id<MTLRenderPipelineState> handle;
};

#endif //AGPU_METAL_PIPELINE_STATE_HPP
