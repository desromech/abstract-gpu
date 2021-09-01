#include "vertex_layout.hpp"
#include "texture_format.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{
    
AMtlVertexLayout::AMtlVertexLayout(const agpu::device_ref &device)
    : device(device)
{
    AgpuProfileConstructor(AMtlVertexLayout);
}

AMtlVertexLayout::~AMtlVertexLayout()
{
    AgpuProfileDestructor(AMtlVertexLayout);
}

agpu::vertex_layout_ref AMtlVertexLayout::create (const agpu::device_ref &device)
{
    return agpu::makeObject<AMtlVertexLayout> (device);
}

agpu_error AMtlVertexLayout::addVertexAttributeBindings ( agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
    CHECK_POINTER(vertex_strides);
    CHECK_POINTER(attributes);

    // Store the new strides
    auto startBuffer = this->vertexStrides.size();
    this->vertexStrides.reserve(vertex_buffer_count);
    this->vertexAttributes.reserve(attribute_count);
    for(size_t i = 0; i < vertex_buffer_count; ++i)
        this->vertexStrides.push_back(vertex_strides[i]);

    // Store the newer attributes.
    for(size_t i = 0; i < attribute_count; ++i)
    {
        auto attribute = attributes[i];
        attribute.buffer += startBuffer;
        this->vertexAttributes.push_back(attribute);
    }

    return AGPU_OK;
}

MTLVertexDescriptor *AMtlVertexLayout::createVertexDescriptor(agpu_uint vertexBufferStartIndex)
{
    auto vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
    if(!vertexDescriptor) return vertexDescriptor;

    // Set the strides.
    for(size_t i = 0; i < vertexStrides.size(); ++i)
    {
        auto desc = vertexDescriptor.layouts[i + vertexBufferStartIndex];
        desc.stride = vertexStrides[i];
        desc.stepFunction = MTLVertexStepFunctionPerVertex;
    }

    // Store the newer attributes.
    for(size_t i = 0; i < vertexAttributes.size(); ++i)
    {
        auto &attribute = vertexAttributes[i];
        auto desc = vertexDescriptor.attributes[attribute.binding];

        desc.format = mapVertexFormat(attribute.format);
        desc.bufferIndex = attribute.buffer + vertexBufferStartIndex;
        desc.offset = attribute.offset;
        if(attribute.divisor != 0)
        {
            auto layout = vertexDescriptor.layouts[attribute.buffer + vertexBufferStartIndex];
            layout.stepFunction = MTLVertexStepFunctionPerInstance;
            layout.stepRate = attribute.divisor;
        }
    }

    return vertexDescriptor;
}

} // End of namespace AgpuMetal
