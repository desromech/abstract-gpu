#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "vertex_binding.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "shader_resource_binding.hpp"

inline GLenum mapPrimitiveTopology(agpu_primitive_topology topology)
{
    switch (topology)
    {
    case AGPU_POINTS: return GL_POINTS;
    case AGPU_LINES: return GL_LINES;
    case AGPU_LINES_ADJACENCY: return GL_LINES_ADJACENCY;
    case AGPU_LINE_STRIP: return GL_LINE_STRIP;
    case AGPU_LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
    case AGPU_TRIANGLES: return GL_TRIANGLES;
    case AGPU_TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
    case AGPU_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case AGPU_TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
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

agpu_command_list *_agpu_command_list::create(agpu_device *device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    auto list = new agpu_command_list();
    list->device = device;
    list->type = type;;
    if(initial_pipeline_state)
        list->usePipelineState(initial_pipeline_state);
    return list;
}

agpu_error _agpu_command_list::setShaderSignature(agpu_shader_signature* signature)
{
    return AGPU_OK;
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

agpu_error _agpu_command_list::setPrimitiveTopology(agpu_primitive_topology topology)
{
    GLenum newPrimitiveMode = mapPrimitiveTopology(topology);
    return addCommand([=] {
        primitiveMode = newPrimitiveMode;
    });
}

agpu_error _agpu_command_list::useShaderResources ( agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(binding);
    return addCommand([=] {
        binding->activate();
    });
}

agpu_error _agpu_command_list::drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    return addCommand([=] {
        if (!currentVertexBinding)
            return;

        currentVertexBinding->bind();
        device->glDrawArraysInstancedBaseInstance(primitiveMode, first_vertex, vertex_count, instance_count, base_instance);
    });
}

agpu_error _agpu_command_list::drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer)
            return;

        currentVertexBinding->bind();
        currentIndexBuffer->bind();
        size_t offset = currentIndexBuffer->description.stride*first_index;
        device->glDrawElementsInstancedBaseVertexBaseInstance(primitiveMode, index_count,
            mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> (offset),
            instance_count, base_vertex, base_instance);
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

        device->glDrawElementsIndirect(primitiveMode, mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset));
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

        device->glMultiDrawElementsIndirect(primitiveMode, mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset), (GLsizei)drawcount, currentDrawBuffer->description.stride);
    });
}

agpu_error _agpu_command_list::setStencilReference(agpu_uint reference)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::executeBundle ( agpu_command_list* bundle )
{
    CHECK_POINTER(bundle)
    if(bundle->type != AGPU_COMMAND_LIST_TYPE_BUNDLE)
        return AGPU_INVALID_PARAMETER;

    return addCommand([=] {
        bundle->execute();
    });
}

agpu_error _agpu_command_list::close()
{
    closed = true;
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset(agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    closed = false;
    commands.clear();
    if (initial_pipeline_state)
        usePipelineState(initial_pipeline_state);
    return AGPU_OK;
}

agpu_error _agpu_command_list::beginFrame (agpu_framebuffer* framebuffer)
{
    CHECK_POINTER(framebuffer)
    return addCommand([=] {
        framebuffer->bind();
    });
}

agpu_error _agpu_command_list::endFrame()
{
    return addCommand([=] {
        device->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
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

AGPU_EXPORT agpu_error agpuSetShaderSignature(agpu_command_list* command_list, agpu_shader_signature* signature)
{
    CHECK_POINTER(command_list);
    return command_list->setShaderSignature(signature);
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

AGPU_EXPORT agpu_error agpuSetPrimitiveTopology(agpu_command_list* command_list, agpu_primitive_topology topology)
{
    CHECK_POINTER(command_list);
    return command_list->setPrimitiveTopology(topology);
}

AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(command_list);
    return command_list->useShaderResources(binding);
}

AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    CHECK_POINTER(command_list);
    return command_list->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    CHECK_POINTER(command_list);
    return command_list->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
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

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_uint reference)
{
    CHECK_POINTER(command_list);
    return command_list->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->close();
}

AGPU_EXPORT agpu_error agpuExecuteBundle ( agpu_command_list* command_list, agpu_command_list* bundle )
{
    CHECK_POINTER(command_list);
    return command_list->executeBundle(bundle);
}

AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    CHECK_POINTER(command_list);
    return command_list->reset(allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_error agpuBeginFrame ( agpu_command_list* command_list, agpu_framebuffer* framebuffer )
{
    CHECK_POINTER(command_list);
    return command_list->beginFrame(framebuffer);
}

AGPU_EXPORT agpu_error agpuEndFrame(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->endFrame();
}
