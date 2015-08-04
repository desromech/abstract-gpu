#ifndef AGPU_D3D12_PIPELINE_STATE_HPP
#define AGPU_D3D12_PIPELINE_STATE_HPP

#include "device.hpp"

struct _agpu_pipeline_state : public Object<_agpu_pipeline_state>
{
public:
    _agpu_pipeline_state();

    void lostReferences();

    agpu_int getUniformLocation(agpu_cstring name);

public:
    agpu_device *device;
    ComPtr<ID3D12PipelineState> state;
};

#endif //AGPU_D3D12_PIPELINE_STATE_HPP
