#ifndef AGPU_COMMAND_LIST_HPP_
#define AGPU_COMMAND_LIST_HPP_

#include <vector>
#include <functional>
#include "device.hpp"

typedef std::function<void()> AgpuGLCommand;

struct _agpu_command_list: public Object<_agpu_command_list>
{
public:
    _agpu_command_list();
    
    static agpu_command_list *create(agpu_device *device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);

    void lostReferences();

    agpu_error setShaderSignature(agpu_shader_signature* signature);
    agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h);
    agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h);
    agpu_error setClearColor(agpu_float r, agpu_float g, agpu_float b, agpu_float a);
    agpu_error setClearDepth(agpu_float depth);
    agpu_error setClearStencil(agpu_int value);
    agpu_error clear(agpu_bitfield buffers);
    agpu_error usePipelineState(agpu_pipeline_state* pipeline);
    agpu_error useVertexBinding(agpu_vertex_binding* vertex_binding);
    agpu_error useIndexBuffer(agpu_buffer* index_buffer);
    agpu_error useDrawIndirectBuffer(agpu_buffer* draw_buffer);
    agpu_error setPrimitiveTopology(agpu_primitive_topology topology);
    agpu_error useShaderResources ( agpu_shader_resource_binding* binding );
    agpu_error drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance );
    agpu_error drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance );
    agpu_error drawElementsIndirect(agpu_size offset);
    agpu_error multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount);
    agpu_error setStencilReference(agpu_uint reference);
    agpu_error executeBundle ( agpu_command_list* bundle );
    agpu_error close();
    agpu_error reset(agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);
    agpu_error beginRenderPass(agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content);
    agpu_error endRenderPass();
    agpu_error resolveFramebuffer(agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer);
        
public:
    agpu_device *device;
    
    agpu_pipeline_state *currentPipeline;

    agpu_vertex_binding *currentVertexBinding;
    agpu_buffer *currentIndexBuffer;
    agpu_buffer *currentDrawBuffer;
    GLenum primitiveMode;
    agpu_command_list_type type;

    agpu_uint stencilReference;

    void execute();

private:
    agpu_error addCommand(const AgpuGLCommand &command);

    std::vector<AgpuGLCommand> commands;
    bool closed;
};


#endif //AGPU_COMMAND_LIST_HPP_
