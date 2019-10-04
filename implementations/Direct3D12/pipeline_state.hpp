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

	virtual void activatedOnCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList);

public:
    agpu::device_ref device;
    ComPtr<ID3D12PipelineState> state;
};

class ADXGraphicsPipelineState : public ADXPipelineState
{
public:
	ADXGraphicsPipelineState();
	~ADXGraphicsPipelineState();

	virtual void activatedOnCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList) override;

	agpu_primitive_topology primitiveTopology;
};

class ADXComputePipelineState : public ADXPipelineState
{
public:
	ADXComputePipelineState();
	~ADXComputePipelineState();
};
} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_PIPELINE_STATE_HPP
