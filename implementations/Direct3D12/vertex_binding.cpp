#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "buffer.hpp"

_agpu_vertex_binding::_agpu_vertex_binding()
{

}

void _agpu_vertex_binding::lostReferences()
{
    // Release vertex buffer references.
    for (auto buffer : vertexBuffers)
    {
        if (buffer)
            buffer->release();
    }
}

agpu_vertex_binding* _agpu_vertex_binding::create(agpu_device* device, agpu_vertex_layout *layout)
{
    if (!layout)
        return nullptr;

    auto binding  = new agpu_vertex_binding();
    binding->device = device;
    binding->layout = layout;
    binding->vertexBufferViews.resize(layout->vertexBufferCount);
    binding->vertexBuffers.resize(layout->vertexBufferCount);
    return binding;
}

agpu_error _agpu_vertex_binding::bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers)
{
    CHECK_POINTER(vertex_buffers);
    if (count != layout->vertexBufferCount)
        return AGPU_ERROR;

    for (agpu_uint i = 0; i < count; ++i)
    {
        auto buffer = vertex_buffers[i];
        if (!buffer)
            return AGPU_ERROR;

        // Store a reference to the vertex buffer
        buffer->retain();
        if (vertexBuffers[i])
            vertexBuffers[i]->release();
        vertexBuffers[i] = buffer;

        // Store the view.
        vertexBufferViews[i] = buffer->view.vertexBuffer;
    }

    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddVertexBindingReference(agpu_vertex_binding* vertex_binding)
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding(agpu_vertex_binding* vertex_binding)
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->release();
}

AGPU_EXPORT agpu_error agpuBindVertexBuffers(agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers)
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->bindVertexBuffers(count, vertex_buffers);
}
