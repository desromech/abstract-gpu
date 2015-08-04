#ifndef AGPU_D3D12_COMMAND_LIST_HPP_
#define AGPU_D3D12_COMMAND_LIST_HPP_

#include "device.hpp"

struct _agpu_command_list : public Object<_agpu_command_list>
{
public:
    _agpu_command_list();

    void lostReferences();

    static _agpu_command_list *create(agpu_device *device, _agpu_command_allocator *allocator, agpu_pipeline_state *initialState);

    agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h);
    agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h);
    agpu_error setClearColor(agpu_float r, agpu_float g, agpu_float b, agpu_float a);
    agpu_error setClearDepth(agpu_float depth);
    agpu_error setClearStencil(agpu_int value);
    agpu_error clear(agpu_bitfield buffers);
    agpu_error usePipelineState(agpu_pipeline_state* pipeline);
    agpu_error useVertexBinding(agpu_vertex_binding* vertex_binding);
    agpu_error useIndexBuffer(agpu_buffer* index_buffer);
    agpu_error setPrimitiveTopology(agpu_primitive_topology topology);
    agpu_error useDrawIndirectBuffer(agpu_buffer* draw_buffer);
    agpu_error useShaderResources(agpu_shader_resource_binding* binding);
    agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
    agpu_error drawElementsIndirect(agpu_size offset);
    agpu_error multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount);
    agpu_error setStencilReference(agpu_float reference);
    agpu_error setAlphaReference(agpu_float reference);
    agpu_error setUniformi(agpu_int location, agpu_size count, agpu_int* data);
    agpu_error setUniform2i(agpu_int location, agpu_size count, agpu_int* data);
    agpu_error setUniform3i(agpu_int location, agpu_size count, agpu_int* data);
    agpu_error setUniform4i(agpu_int location, agpu_size count, agpu_int* data);
    agpu_error setUniformf(agpu_int location, agpu_size count, agpu_float* data);
    agpu_error setUniform2f(agpu_int location, agpu_size count, agpu_float* data);
    agpu_error setUniform3f(agpu_int location, agpu_size count, agpu_float* data);
    agpu_error setUniform4f(agpu_int location, agpu_size count, agpu_float* data);
    agpu_error setUniformMatrix2f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data);
    agpu_error setUniformMatrix3f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data);
    agpu_error setUniformMatrix4f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data);
    agpu_error close();
    agpu_error reset(_agpu_command_allocator *allocator, agpu_pipeline_state* initial_pipeline_state);
    agpu_error beginFrame();
    agpu_error endFrame();

public:
    agpu_device *device;
    ComPtr<ID3D12GraphicsCommandList> commandList;

    // Clearing state.
    float clearColor[4];
    float clearDepth;
    int clearStencil;

private:
    agpu_error setCommonState();
};

#endif //AGPU_D3D12_COMMAND_LIST_HPP_