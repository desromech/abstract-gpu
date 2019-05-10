#include "vertex_layout.hpp"
#include "texture_format.hpp"

namespace AgpuMetal
{
    
AMtlVertexLayout::AMtlVertexLayout(const agpu::device_ref &device)
    : device(device)
{
    vertexDescriptor = nil;
}

AMtlVertexLayout::~AMtlVertexLayout()
{
}

agpu::vertex_layout_ref AMtlVertexLayout::create (const agpu::device_ref &device)
{
    auto descriptor = [MTLVertexDescriptor vertexDescriptor];
    if(!descriptor)
        return agpu::vertex_layout_ref();

    auto result = agpu::makeObject<AMtlVertexLayout> (device);
    auto vertexLayout = result.as<AMtlVertexLayout> ();
    vertexLayout->vertexDescriptor = descriptor;
    return result;
}

agpu_error AMtlVertexLayout::addVertexAttributeBindings ( agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
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

} // End of namespace AgpuMetal
