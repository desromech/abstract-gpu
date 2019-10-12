#include "pipeline_state.hpp"

namespace AgpuD3D12
{
inline D3D_PRIMITIVE_TOPOLOGY mapPrimitiveTopology(agpu_primitive_topology topology)
{
	switch (topology)
	{
	case AGPU_POINTS: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case AGPU_LINES: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case AGPU_LINES_ADJACENCY: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case AGPU_LINE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case AGPU_LINE_STRIP_ADJACENCY:return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case AGPU_TRIANGLES: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case AGPU_TRIANGLES_ADJACENCY: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case AGPU_TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case AGPU_TRIANGLE_STRIP_ADJACENCY: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	default:
		abort();
	}
}

// Generic implementation for a pipeline state.
ADXPipelineState::ADXPipelineState()
{
}

ADXPipelineState::~ADXPipelineState()
{
}

void ADXPipelineState::activatedOnCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
}

// Graphics pipeline state specific functionality.
ADXGraphicsPipelineState::ADXGraphicsPipelineState()
	: primitiveTopology(AGPU_POINTS)
{
}

ADXGraphicsPipelineState::~ADXGraphicsPipelineState()
{
}

void ADXGraphicsPipelineState::activatedOnCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->IASetPrimitiveTopology(mapPrimitiveTopology(primitiveTopology));
}

// Compute pipeline state specific functionality.
ADXComputePipelineState::ADXComputePipelineState()
{
}

ADXComputePipelineState::~ADXComputePipelineState()
{
}

} // End of namespace AgpuD3D12
