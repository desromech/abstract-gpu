#ifndef AGPU_D3D12_PIPELINE_STATE_HPP
#define AGPU_D3D12_PIPELINE_STATE_HPP

#include "device.hpp"

namespace AgpuD3D12
{

class ADXPipelineState : public agpu::pipeline_state
{
public:
    ADXPipelineState();
    ~ADXPipelineState();

public:
    agpu::device_ref device;
    ComPtr<ID3D12PipelineState> state;
    agpu_primitive_topology primitiveTopology;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_PIPELINE_STATE_HPP
