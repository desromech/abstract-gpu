#ifndef AGPU_D3D12_COMMAND_LIST_HPP_
#define AGPU_D3D12_COMMAND_LIST_HPP_

#include "device.hpp"

enum class CommandListType
{
    Direct = 0,
    Bundle,
    Compute
};

struct _agpu_command_list : public Object<_agpu_command_list>
{
public:
    _agpu_command_list();

    void lostReferences();

    static _agpu_command_list *create(agpu_device *device, CommandListType type, _agpu_command_allocator *allocator, agpu_pipeline_state *initialState);

    agpu_error setShaderSignature(agpu_shader_signature *signature);

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
    agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
    agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
    agpu_error drawElementsIndirect(agpu_size offset);
    agpu_error multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount);
    agpu_error setStencilReference(agpu_uint reference);
    agpu_error executeBundle(agpu_command_list* bundle);
    agpu_error close();
    agpu_error reset(_agpu_command_allocator *allocator, agpu_pipeline_state* initial_pipeline_state);
    agpu_error beginFrame(agpu_framebuffer* framebuffer);
    agpu_error endFrame();

public:
    agpu_device *device;
    ComPtr<ID3D12GraphicsCommandList> commandList;

    // Some flags
    CommandListType type;

    // Clearing state.
    float clearColor[4];
    float clearDepth;
    int clearStencil;

    // Framebuffer
    agpu_framebuffer *currentFramebuffer;

private:
    agpu_error setCommonState();
};

#endif //AGPU_D3D12_COMMAND_LIST_HPP_