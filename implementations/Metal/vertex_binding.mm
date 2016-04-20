#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "buffer.hpp"

_agpu_vertex_binding::_agpu_vertex_binding(agpu_device *device)
    : device(device)
{
}

void _agpu_vertex_binding::lostReferences()
{
    for(auto buffer : buffers)
    {
        if(buffer)
            buffer->release();
    }
}

_agpu_vertex_binding *_agpu_vertex_binding::create(agpu_device *device, agpu_vertex_layout *layout)
{
    if(!layout)
        return nullptr;

    auto result = new agpu_vertex_binding(device);
    result->buffers.resize(layout->vertexStrides.size());
    return result;
}

agpu_error _agpu_vertex_binding::bindBuffers ( agpu_uint count, agpu_buffer** vertex_buffers )
{
    CHECK_POINTER(vertex_buffers);
    if(count != buffers.size())
        return AGPU_INVALID_PARAMETER;
    for(size_t i = 0; i < count; ++i)
    {
        auto newBuffer = vertex_buffers[i];
        if(newBuffer)
            newBuffer->retain();
        if(buffers[i])
            buffers[i]->release();
        buffers[i] = newBuffer;
    }

    return AGPU_OK;
}

// The exported C interface.
AGPU_EXPORT agpu_error agpuAddVertexBindingReference ( agpu_vertex_binding* vertex_binding )
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding ( agpu_vertex_binding* vertex_binding )
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->release();
}

AGPU_EXPORT agpu_error agpuBindVertexBuffers ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers )
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->bindBuffers(count, vertex_buffers);
}
