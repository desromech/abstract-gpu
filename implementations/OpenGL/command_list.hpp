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
    
    static agpu_command_list *create(agpu_device *device, agpu_pipeline_state* initial_pipeline_state);

    void lostReferences();

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
    agpu_error reset(agpu_pipeline_state* initial_pipeline_state);
    agpu_error beginFrame();
    agpu_error endFrame();
        
public:
    agpu_device *device;
    
    agpu_pipeline_state *currentPipeline;

    agpu_vertex_binding *currentVertexBinding;
    agpu_buffer *currentIndexBuffer;
    agpu_buffer *currentDrawBuffer;

    void execute();

private:
    agpu_error addCommand(const AgpuGLCommand &command);

    std::vector<AgpuGLCommand> commands;
    bool closed;
};


#endif //AGPU_COMMAND_LIST_HPP_