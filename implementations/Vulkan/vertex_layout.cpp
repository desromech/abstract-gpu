#include "vertex_layout.hpp"
#include "texture_format.hpp"
#include <algorithm>

namespace AgpuVulkan
{

AVkVertexLayout::AVkVertexLayout(const agpu::device_ref &device)
    : device(device)
{
}

AVkVertexLayout::~AVkVertexLayout()
{
}

agpu::vertex_layout_ref AVkVertexLayout::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AVkVertexLayout> (device);
}

agpu_error AVkVertexLayout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
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

        if (attribute.format == AGPU_TEXTURE_FORMAT_UNKNOWN)
            return AGPU_INVALID_PARAMETER;

        attribute.buffer += (agpu_uint)baseBuffer;
        bufferDimensions[attribute.buffer].divisor = std::max(bufferDimensions[attribute.buffer].divisor, attribute.divisor);

        allAttributes.push_back(attribute);
    }

    return AGPU_OK;
}

} // End of namespace AgpuVulkan
