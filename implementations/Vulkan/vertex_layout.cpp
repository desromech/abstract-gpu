#include "vertex_layout.hpp"
#include "texture_format.hpp"
#include <algorithm>

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

agpu_error _agpu_vertex_layout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(vertex_strides);
    CHECK_POINTER(attributes);

    auto baseBuffer = bufferDimensions.size();
    for (size_t i = 0; i < vertex_buffer_count; ++i)
        bufferDimensions.push_back(VertexStructureDimensions(vertex_strides[i]));

    allAttributes.reserve(attribute_count);
    for (size_t i = 0; i < attribute_count; ++i)
    {
        auto attribute = attributes[i];
        if (attribute.buffer >= vertex_buffer_count)
            return AGPU_INVALID_PARAMETER;

        attribute.buffer += baseBuffer;
        bufferDimensions[attribute.buffer].divisor = std::max(bufferDimensions[attribute.buffer].divisor, attribute.divisor);

        allAttributes.push_back(attribute);
    }

    return AGPU_OK;
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

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings(agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->addVertexAttributeBindings(vertex_buffer_count, vertex_strides, attribute_count, attributes);
}
