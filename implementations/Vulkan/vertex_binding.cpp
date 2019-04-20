#include "vertex_binding.hpp"
#include "buffer.hpp"

_agpu_vertex_binding::_agpu_vertex_binding(agpu_device *device)
    : device(device)
{
}

void _agpu_vertex_binding::lostReferences()
{
    for (auto buffer : vertexBuffers)
        buffer->release();
}

agpu_vertex_binding *_agpu_vertex_binding::create(agpu_device *device, agpu_vertex_layout* layout)
{
    return new agpu_vertex_binding(device);
}

agpu_error _agpu_vertex_binding::bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers)
{
    return bindVertexBuffersWithOffsets(count, vertex_buffers, nullptr);
}


agpu_error _agpu_vertex_binding::bindVertexBuffersWithOffsets(agpu_uint count, agpu_buffer** vertex_buffers, agpu_size *offsets)
{
    for (size_t i = 0; i < count; ++i)
    {
        CHECK_POINTER(vertex_buffers[i]);
        if ((vertex_buffers[i]->description.binding & AGPU_ARRAY_BUFFER) == 0)
            return AGPU_INVALID_PARAMETER;
    }

    for (size_t i = 0; i < count; ++i)
        vertex_buffers[i]->retain();
    for(auto buffer : vertexBuffers)
    {
        if(buffer)
            buffer->release();
    }

    this->vertexBuffers.resize(count);
    this->vulkanBuffers.resize(count);
    this->offsets.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        auto buffer = vertex_buffers[i];
        buffer->retain();
        this->vertexBuffers[i] = buffer;
        this->vulkanBuffers[i] = buffer->getDrawBuffer();
        this->offsets[i] = offsets ? offsets[i] : 0;
    }

    return AGPU_OK;
}

// The exported C interface
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

AGPU_EXPORT agpu_error agpuBindVertexBuffersWithOffsets ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers, agpu_size* offsets )
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->bindVertexBuffersWithOffsets(count, vertex_buffers, offsets);
}
