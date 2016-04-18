#include "vertex_layout.hpp"

_agpu_vertex_layout::_agpu_vertex_layout(agpu_device *device)
    : device(device)
{
}

void _agpu_vertex_layout::lostReferences()
{
}

agpu_vertex_layout* _agpu_vertex_layout::create ( agpu_device* device )
{
    return new agpu_vertex_layout(device);
}

agpu_error _agpu_vertex_layout::addVertexAttributeBindings ( agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
    CHECK_POINTER(vertex_strides);
    CHECK_POINTER(attributes);

    // Store the new strides
    this->vertexStrides.reserve(vertex_buffer_count);
    for(size_t i = 0; i < vertex_buffer_count; ++i)
        this->vertexStrides.push_back(vertex_strides[i]);

    // Store the newer attributes.
    this->allAttributes.reserve(attribute_count);
    for(size_t i = 0; i < attribute_count; ++i)
        this->allAttributes.push_back(attributes[i]);

    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddVertexLayoutReference ( agpu_vertex_layout* vertex_layout )
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVertexLayout ( agpu_vertex_layout* vertex_layout )
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->release();
}

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->addVertexAttributeBindings(vertex_buffer_count, vertex_strides, attribute_count, attributes);
}
