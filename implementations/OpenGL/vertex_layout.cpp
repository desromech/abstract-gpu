#include "vertex_layout.hpp"

namespace AgpuGL
{

GLVertexLayout::GLVertexLayout()
{
    vertexBufferCount = 0;
}

GLVertexLayout::~GLVertexLayout()
{
}

agpu::vertex_layout_ref GLVertexLayout::createVertexLayout(const agpu::device_ref &device)
{
    auto result = agpu::makeObject<GLVertexLayout> ();
    result.as<GLVertexLayout> ()->device = device;
    return result;
}

agpu_error GLVertexLayout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    vertexBufferCount = vertex_buffer_count;
    this->strides.reserve(vertex_buffer_count);
    for (size_t i = 0; i < vertex_buffer_count; ++i)
        this->strides.push_back(vertex_strides[i]);

    this->attributes.reserve(attribute_count);
    for (size_t i = 0; i < attribute_count; ++i)
        this->attributes.push_back(attributes[i]);
    return AGPU_OK;
}

} // End of namespace AgpuGL
