#include "command_list.hpp"

_agpu_command_list::_agpu_command_list()
{

}

void _agpu_command_list::lostReferences()
{

}

// Exported C interface

AGPU_EXPORT agpu_error agpuAddCommandListReference(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->release();
}

AGPU_EXPORT agpu_error agpuSetViewport(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetScissor(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetClearColor(agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetClearDepth(agpu_command_list* command_list, agpu_float depth)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetClearStencil(agpu_command_list* command_list, agpu_int value)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuClear(agpu_command_list* command_list, agpu_bitfield buffers)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUsePipelineState(agpu_command_list* command_list, agpu_pipeline_state* pipeline)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseVertexBinding(agpu_command_list* command_list, agpu_vertex_binding* vertex_binding)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer(agpu_command_list* command_list, agpu_buffer* index_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer(agpu_command_list* command_list, agpu_buffer* draw_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetAlphaReference(agpu_command_list* command_list, agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformi(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform2i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform3i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform4i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformf(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform2f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform3f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform4f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix2f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix3f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix4f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* command_list)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuResetCommandList(agpu_command_list* command_list)
{
    return AGPU_UNIMPLEMENTED;
}
