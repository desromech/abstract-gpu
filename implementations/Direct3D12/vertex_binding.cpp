#include "vertex_binding.hpp"

_agpu_vertex_binding::_agpu_vertex_binding()
{

}

void _agpu_vertex_binding::lostReferences()
{

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

AGPU_EXPORT agpu_error agpuAddVertexBufferBindings(agpu_vertex_binding* vertex_binding, agpu_buffer* vertex_buffer, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    return AGPU_UNIMPLEMENTED;
}
