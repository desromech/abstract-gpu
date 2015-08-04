#include "vertex_layout.hpp"

_agpu_vertex_layout::_agpu_vertex_layout()
{
    vertexBufferCount = 0;
}

void _agpu_vertex_layout::lostReferences()
{
}

agpu_vertex_layout *_agpu_vertex_layout::createVertexLayout(agpu_device *device)
{
    auto layout = new agpu_vertex_layout();
    layout->device = device;
    return layout;
}

agpu_error _agpu_vertex_layout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    vertexBufferCount = vertex_buffer_count;
    this->attributes.reserve(attribute_count);
    for (size_t i = 0; i < attribute_count; ++i)
        this->attributes.push_back(attributes[i]);
    return AGPU_OK;
}

// Exported C interface
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
