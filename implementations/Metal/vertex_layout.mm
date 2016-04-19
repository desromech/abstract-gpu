#include "vertex_layout.hpp"
#include "texture_format.hpp"

_agpu_vertex_layout::_agpu_vertex_layout(agpu_device *device)
    : device(device)
{
    vertexDescriptor = nil;
}

void _agpu_vertex_layout::lostReferences()
{
}

agpu_vertex_layout* _agpu_vertex_layout::create ( agpu_device* device )
{
    auto descriptor = [MTLVertexDescriptor vertexDescriptor];
    if(!descriptor)
        return nullptr;

    auto result = new agpu_vertex_layout(device);
    result->vertexDescriptor = descriptor;
    return result;
}

agpu_error _agpu_vertex_layout::addVertexAttributeBindings ( agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
    CHECK_POINTER(vertex_strides);
    CHECK_POINTER(attributes);

    auto startBuffer = this->vertexStrides.size();

    // Store the new strides
    this->vertexStrides.reserve(vertex_buffer_count);
    for(size_t i = 0; i < vertex_buffer_count; ++i)
    {
        auto desc = vertexDescriptor.layouts[i + startBuffer];
        desc.stride = vertex_strides[i];
        desc.stepFunction = MTLVertexStepFunctionPerVertex;

        this->vertexStrides.push_back(vertex_strides[i]);
    }

    // Store the newer attributes.
    for(size_t i = 0; i < attribute_count; ++i)
    {
        auto &attribute = attributes[i];
        auto desc = vertexDescriptor.attributes[attribute.binding];

        desc.format = mapVertexFormat(attribute.format);
        desc.bufferIndex = attribute.buffer;
        desc.offset = attribute.offset;
        if(attribute.divisor != 0 )
        {
            auto layout = vertexDescriptor.layouts[attribute.buffer + startBuffer];
            layout.stepFunction = MTLVertexStepFunctionPerInstance;
            layout.stepRate = attribute.divisor;
        }
    }

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
