#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "vertex_binding.hpp"
#include "buffer.hpp"

inline GLenum mapPrimitiveMode(agpu_primitive_mode mode)
{
    switch (mode)
    {
    case AGPU_POINTS: return GL_POINTS;
    case AGPU_LINES: return GL_LINES;
    case AGPU_LINES_ADJACENCY: return GL_LINES_ADJACENCY;
    case AGPU_LINE_STRIP: return GL_LINE_STRIP;
    case AGPU_LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
    case AGPU_LINE_LOOP: return GL_LINE_LOOP;
    case AGPU_TRIANGLES: return GL_TRIANGLES;
    case AGPU_TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
    case AGPU_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case AGPU_TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
    case AGPU_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
    case AGPU_PATCHES: return GL_PATCHES;
    default: abort();
    }
}

inline GLenum mapIndexType(agpu_size stride)
{
    switch (stride)
    {
    case 1: return GL_UNSIGNED_BYTE;
    case 2: return GL_UNSIGNED_SHORT;
    case 4: return GL_UNSIGNED_INT;
    default: abort();
    }
}

_agpu_command_list::_agpu_command_list()
{
    closed = false;
}

void _agpu_command_list::lostReferences()
{

}

agpu_command_list *_agpu_command_list::create(agpu_device *device, agpu_pipeline_state* initial_pipeline_state)
{
    auto list = new agpu_command_list();
    list->device = device;
    if(initial_pipeline_state)
        list->usePipelineState(initial_pipeline_state);
    return list;
}

agpu_error _agpu_command_list::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return addCommand([=] {
        glViewport(x, y, w, h);
    });
}

agpu_error _agpu_command_list::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return addCommand([=] {
        glScissor(x, y, w, h);
    });
}

agpu_error _agpu_command_list::setClearColor(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    return addCommand([=] {
        glClearColor(r, g, b, a);
    });
}

agpu_error _agpu_command_list::setClearDepth(agpu_float depth)
{
    return addCommand([=] {
        glClearDepth(depth);
    });
}

agpu_error _agpu_command_list::setClearStencil(agpu_int value)
{
    return addCommand([=] {
        glClearStencil(value);
    });
}

agpu_error _agpu_command_list::clear(agpu_bitfield buffers)
{
    GLbitfield bits = 0;
    if (buffers & AGPU_COLOR_BUFFER_BIT)
        bits |= GL_COLOR_BUFFER_BIT;
    if (buffers & AGPU_DEPTH_BUFFER_BIT)
        bits |= GL_DEPTH_BUFFER_BIT;
    if (buffers & AGPU_STENCIL_BUFFER_BIT)
        bits |= GL_STENCIL_BUFFER_BIT;

    return addCommand([=] {
        glClear(bits);
    });
}

agpu_error _agpu_command_list::usePipelineState(agpu_pipeline_state* pipeline)
{
    return addCommand([=] {
        this->currentPipeline = pipeline;
        this->currentPipeline->activate();
    });
}

agpu_error _agpu_command_list::useVertexBinding(agpu_vertex_binding* vertex_binding)
{
    return addCommand([=] {
        this->currentVertexBinding = vertex_binding;
    });
}

agpu_error _agpu_command_list::useIndexBuffer(agpu_buffer* index_buffer)
{
    return addCommand([=] {
        this->currentIndexBuffer= index_buffer;
    });
}

agpu_error _agpu_command_list::useDrawIndirectBuffer(agpu_buffer* draw_buffer)
{
    return addCommand([=] {
        this->currentDrawBuffer = draw_buffer;
    });
}

agpu_error _agpu_command_list::drawElementsIndirect(agpu_size offset)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer || !currentDrawBuffer)
            return;

        currentVertexBinding->bind();
        currentIndexBuffer->bind();
        currentDrawBuffer->bind();

        device->glDrawElementsIndirect(mapPrimitiveMode(currentPipeline->primitiveTopology), mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset));
    });
}

agpu_error _agpu_command_list::multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer || !currentDrawBuffer)
            return;

        currentVertexBinding->bind();
        currentIndexBuffer->bind();
        currentDrawBuffer->bind();

        device->glMultiDrawElementsIndirect(mapPrimitiveMode(currentPipeline->primitiveTopology), mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset), (GLsizei)drawcount, currentDrawBuffer->description.stride);
    });
}

agpu_error _agpu_command_list::setStencilReference(agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setAlphaReference(agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniformi(agpu_int location, agpu_size count, agpu_int* data)
{
    return addCommand([=] {
        device->glUniform1iv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniform2i(agpu_int location, agpu_size count, agpu_int* data)
{
    return addCommand([=] {
        device->glUniform2iv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniform3i(agpu_int location, agpu_size count, agpu_int* data)
{
    return addCommand([=] {
        device->glUniform3iv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniform4i(agpu_int location, agpu_size count, agpu_int* data)
{
    return addCommand([=] {
        device->glUniform4iv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniformf(agpu_int location, agpu_size count, agpu_float* data)
{
    return addCommand([=] {
        device->glUniform1fv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniform2f(agpu_int location, agpu_size count, agpu_float* data)
{
    return addCommand([=] {
        device->glUniform2fv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniform3f(agpu_int location, agpu_size count, agpu_float* data)
{
    return addCommand([=] {
        device->glUniform3fv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniform4f(agpu_int location, agpu_size count, agpu_float* data)
{
    return addCommand([=] {
        device->glUniform4fv(location, (GLsizei)count, data);
    });
}

agpu_error _agpu_command_list::setUniformMatrix2f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return addCommand([=] {
        device->glUniformMatrix2fv(location, (GLsizei)count, transpose, data);
    });
}

agpu_error _agpu_command_list::setUniformMatrix3f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return addCommand([=] {
        device->glUniformMatrix3fv(location, (GLsizei)count, transpose, data);
    });
}

agpu_error _agpu_command_list::setUniformMatrix4f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return addCommand([=] {
        device->glUniformMatrix4fv(location, (GLsizei)count, transpose, data);
    });
}

agpu_error _agpu_command_list::close()
{
    closed = true;
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset(agpu_pipeline_state* initial_pipeline_state)
{
    closed = false;
    commands.clear();
    if (initial_pipeline_state)
        usePipelineState(initial_pipeline_state);
    return AGPU_OK;
}

agpu_error _agpu_command_list::beginFrame()
{
    return AGPU_OK;
}

agpu_error _agpu_command_list::endFrame()
{
    return AGPU_OK;
}

agpu_error _agpu_command_list::addCommand(const AgpuGLCommand &command)
{
    if (closed)
        return AGPU_COMMAND_LIST_CLOSED;

    commands.push_back(command);
    return AGPU_OK;
}

void _agpu_command_list::execute()
{
    for (auto &command : commands)
        command();
}

// C API
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
    CHECK_POINTER(command_list);
    return command_list->setViewport(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuSetScissor(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    CHECK_POINTER(command_list);
    return command_list->setScissor(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuSetClearColor(agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    CHECK_POINTER(command_list);
    return command_list->setClearColor(r, g, b, a);
}

AGPU_EXPORT agpu_error agpuSetClearDepth(agpu_command_list* command_list, agpu_float depth)
{
    CHECK_POINTER(command_list);
    return command_list->setClearDepth(depth);
}

AGPU_EXPORT agpu_error agpuSetClearStencil(agpu_command_list* command_list, agpu_int value)
{
    CHECK_POINTER(command_list);
    return command_list->setClearStencil(value);
}

AGPU_EXPORT agpu_error agpuClear(agpu_command_list* command_list, agpu_bitfield buffers)
{
    CHECK_POINTER(command_list);
    return command_list->clear(buffers);
}

AGPU_EXPORT agpu_error agpuUsePipelineState(agpu_command_list* command_list, agpu_pipeline_state* pipeline)
{
    CHECK_POINTER(command_list);
    return command_list->usePipelineState(pipeline);
}

AGPU_EXPORT agpu_error agpuUseVertexBinding(agpu_command_list* command_list, agpu_vertex_binding* vertex_binding)
{
    CHECK_POINTER(command_list);
    return command_list->useVertexBinding(vertex_binding);
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer(agpu_command_list* command_list, agpu_buffer* index_buffer)
{
    CHECK_POINTER(command_list);
    return command_list->useIndexBuffer(index_buffer);
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer(agpu_command_list* command_list, agpu_buffer* draw_buffer)
{
    CHECK_POINTER(command_list);
    return command_list->useDrawIndirectBuffer(draw_buffer);
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset)
{
    CHECK_POINTER(command_list);
    return command_list->drawElementsIndirect(offset);
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount)
{
    CHECK_POINTER(command_list);
    return command_list->multiDrawElementsIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_float reference)
{
    CHECK_POINTER(command_list);
    return command_list->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuSetAlphaReference(agpu_command_list* command_list, agpu_float reference)
{
    CHECK_POINTER(command_list);
    return command_list->setAlphaReference(reference);
}

AGPU_EXPORT agpu_error agpuSetUniformi(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniformi(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform2i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniform2i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform3i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniform3i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform4i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniform4i(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniformf(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniformf(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform2f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniform2f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform3f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniform3f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniform4f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniform4f(location, count, data);
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix2f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniformMatrix2f(location, count, transpose, data);
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix3f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniformMatrix3f(location, count, transpose, data);
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix4f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    CHECK_POINTER(command_list);
    return command_list->setUniformMatrix4f(location, count, transpose, data);
}

AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->close();
}

AGPU_EXPORT agpu_error agpuResetCommandList(agpu_command_list* command_list, agpu_pipeline_state* initial_pipeline_state)
{
    CHECK_POINTER(command_list);
    return command_list->reset(initial_pipeline_state);
}

AGPU_EXPORT agpu_error agpuBeginFrame(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->beginFrame();
}

AGPU_EXPORT agpu_error agpuEndFrame(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->endFrame();
}
