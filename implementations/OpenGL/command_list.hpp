#ifndef AGPU_COMMAND_LIST_HPP_
#define AGPU_COMMAND_LIST_HPP_

#include <vector>
#include <functional>
#include "device.hpp"

typedef std::function<void()> AgpuGLCommand;

struct CommandListExecutionContext
{
    static constexpr unsigned int MaxNumberOfShaderResourceBindings = 16;

    CommandListExecutionContext();
    void lostReferences();

    void reset();
    void validateBeforeDrawCall();
    void usePipelineState(agpu_pipeline_state* newPipeline);
    void setStencilReference(agpu_uint reference);
    void useShaderResources ( agpu_shader_resource_binding* binding );

    agpu_device *device;
    agpu_pipeline_state *currentPipeline;
    agpu_pipeline_state *activePipeline;
    agpu_shader_resource_binding *shaderResourceBindings[MaxNumberOfShaderResourceBindings];

    bool hasValidActivePipeline;
    bool hasValidShaderResources;
    GLenum primitiveMode;
    agpu_uint stencilReference;
};

struct _agpu_command_list: public Object<_agpu_command_list>
{
public:
    _agpu_command_list();

    static agpu_command_list *create(agpu_device *device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);

    void lostReferences();

    agpu_error setShaderSignature(agpu_shader_signature* signature);
    agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h);
    agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h);
    agpu_error usePipelineState(agpu_pipeline_state* pipeline);
    agpu_error useVertexBinding(agpu_vertex_binding* vertex_binding);
    agpu_error useIndexBuffer(agpu_buffer* index_buffer);
    agpu_error useDrawIndirectBuffer(agpu_buffer* draw_buffer);
    agpu_error useShaderResources ( agpu_shader_resource_binding* binding );
    agpu_error pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values );
    agpu_error drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance );
    agpu_error drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance );
    agpu_error drawElementsIndirect(agpu_size offset);
    agpu_error multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount);
    agpu_error setStencilReference(agpu_uint reference);
    agpu_error executeBundle ( agpu_command_list* bundle );
    agpu_error close();
    agpu_error reset(agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);
    agpu_error resetBundleCommandList ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info );
    agpu_error beginRenderPass(agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content);
    agpu_error endRenderPass();
    agpu_error resolveFramebuffer(agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer);

public:
    agpu_device *device;

    agpu_pipeline_state *currentPipeline;

    agpu_vertex_binding *currentVertexBinding;
    agpu_buffer *currentIndexBuffer;
    agpu_buffer *currentDrawBuffer;
    agpu_command_list_type type;

    void execute();

private:
    agpu_error addCommand(const AgpuGLCommand &command);

    std::vector<AgpuGLCommand> commands;
    bool closed;
    CommandListExecutionContext executionContext;
};


#endif //AGPU_COMMAND_LIST_HPP_
