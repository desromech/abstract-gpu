#include "vertex_layout.hpp"

_agpu_vertex_layout::_agpu_vertex_layout(agpu_device *device)
    : device(device)
{
}

void _agpu_vertex_layout::lostReferences()
{
}

_agpu_vertex_layout *_agpu_vertex_layout::create(agpu_device *device)
{
    std::unique_ptr<_agpu_vertex_layout> result(new _agpu_vertex_layout(device));
    return result.release();
}

agpu_error _agpu_vertex_layout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddVertexLayoutReference(agpu_vertex_layout* vertex_layout)
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVertexLayout(agpu_vertex_layout* vertex_layout)
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->release();
}

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings(agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->addVertexAttributeBindings(vertex_buffer_count, attribute_count, attributes);
}
