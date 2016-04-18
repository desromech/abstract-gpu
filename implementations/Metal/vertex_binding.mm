#include "vertex_binding.hpp"
#include "vertex_layout.hpp"

_agpu_vertex_binding::_agpu_vertex_binding(agpu_device *device)
    : device(device)
{
    layout = nullptr;
}

void _agpu_vertex_binding::lostReferences()
{
    if(layout)
        layout->release();
}

_agpu_vertex_binding *_agpu_vertex_binding::create(agpu_device *device, agpu_vertex_layout *layout)
{
    if(!layout)
        return nullptr;

    layout->retain();
    auto result = new agpu_vertex_binding(device);
    result->layout = layout;
    return result;
}

agpu_error _agpu_vertex_binding::bindBuffers ( agpu_uint count, agpu_buffer** vertex_buffers )
{
    CHECK_POINTER(vertex_buffers);
    return AGPU_UNIMPLEMENTED;
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
