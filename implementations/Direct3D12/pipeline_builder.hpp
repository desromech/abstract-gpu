#ifndef AGPU_D3D12_PIPELINE_BUILDER_HPP
#define AGPU_D3D12_PIPELINE_BUILDER_HPP

#include "device.hpp"

struct _agpu_pipeline_builder : public Object<_agpu_pipeline_builder>
{
public:
    _agpu_pipeline_builder();

    void lostReferences();

    static _agpu_pipeline_builder *create(agpu_device *device);

    agpu_pipeline_state* buildPipelineState();
    agpu_error attachShader(agpu_shader* shader);
    agpu_size getPipelineBuildingLogLength();
    agpu_error getPipelineBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer);
    agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
    agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
    agpu_error setRenderTargetCount(agpu_int count);
    agpu_error setPrimitiveType(agpu_primitive_type type);
    agpu_error setVertexLayout(agpu_vertex_layout* layout);

    agpu_error setRenderTargetFormat(agpu_uint index, agpu_texture_format format);
    agpu_error setDepthStencilFormat(agpu_texture_format format);

public:
    agpu_device *device;
    agpu_vertex_layout *vertexLayout;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC description;

    std::string buildingLog;
};

#endif //AGPU_D3D12_PIPELINE_BUILDER_HPP