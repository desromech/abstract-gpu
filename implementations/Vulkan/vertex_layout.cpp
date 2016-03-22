#include "vertex_layout.hpp"
#include "texture_format.hpp"
#include <algorithm>

inline agpu_texture_format mapImageFormat(agpu_field_type type, int components, bool normalized)
{
    switch (type)
    {
    case AGPU_FLOAT:
        switch (components)
        {
        case 1: return AGPU_TEXTURE_FORMAT_R32_FLOAT;
        case 2: return AGPU_TEXTURE_FORMAT_R32G32_FLOAT;
        case 3: return AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT;
        case 4: return AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT;
        }
    case AGPU_HALF_FLOAT:
        switch (components)
        {
        case 1: return AGPU_TEXTURE_FORMAT_R16_FLOAT;
        case 2: return AGPU_TEXTURE_FORMAT_R16G16_FLOAT;
        case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
        case 4: return AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT;
        }
    case AGPU_DOUBLE: return AGPU_TEXTURE_FORMAT_UNKNOWN;
    case AGPU_FIXED: return AGPU_TEXTURE_FORMAT_UNKNOWN;
    case AGPU_BYTE:
        if (normalized)
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R8_SNORM;
            case 2: return AGPU_TEXTURE_FORMAT_R8G8_SNORM;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R8G8B8A8_SNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R8_SINT;
            case 2: return AGPU_TEXTURE_FORMAT_R8G8_SINT;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R8G8B8A8_SINT;
            }
        }
    case AGPU_UNSIGNED_BYTE:
        if (normalized)
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R8_UNORM;
            case 2: return AGPU_TEXTURE_FORMAT_R8G8_UNORM;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R8_UINT;
            case 2: return AGPU_TEXTURE_FORMAT_R8G8_UINT;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R8G8B8A8_UINT;
            }
        }
    case AGPU_SHORT:
        if (normalized)
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R16_SNORM;
            case 2: return AGPU_TEXTURE_FORMAT_R16G16_SNORM;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R16G16B16A16_SNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R16_SINT;
            case 2: return AGPU_TEXTURE_FORMAT_R16G16_SINT;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R16G16B16A16_SINT;
            }
        }
    case AGPU_UNSIGNED_SHORT:
        if (normalized)
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R16_UNORM;
            case 2: return AGPU_TEXTURE_FORMAT_R16G16_UNORM;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R16G16B16A16_UNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return AGPU_TEXTURE_FORMAT_R16_UINT;
            case 2: return AGPU_TEXTURE_FORMAT_R16G16_UINT;
            case 3: return AGPU_TEXTURE_FORMAT_UNKNOWN;
            case 4: return AGPU_TEXTURE_FORMAT_R16G16B16A16_UINT;
            }
        }
    case AGPU_INT:
        switch (components)
        {
        case 1: return AGPU_TEXTURE_FORMAT_R32_SINT;
        case 2: return AGPU_TEXTURE_FORMAT_R32G32_SINT;
        case 3: return AGPU_TEXTURE_FORMAT_R32G32B32_SINT;
        case 4: return AGPU_TEXTURE_FORMAT_R32G32B32A32_SINT;
        }
    case AGPU_UNSIGNED_INT:
        switch (components)
        {
        case 1: return AGPU_TEXTURE_FORMAT_R32_UINT;
        case 2: return AGPU_TEXTURE_FORMAT_R32G32_UINT;
        case 3: return AGPU_TEXTURE_FORMAT_R32G32B32_UINT;
        case 4: return AGPU_TEXTURE_FORMAT_R32G32B32A32_UINT;
        }
    default:
        return AGPU_TEXTURE_FORMAT_UNKNOWN;
    }
}

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

        if (attribute.internal_format == AGPU_TEXTURE_FORMAT_UNKNOWN)
            attribute.internal_format = mapImageFormat(attribute.type, attribute.components, attribute.normalized);
        if (attribute.internal_format == AGPU_TEXTURE_FORMAT_UNKNOWN)
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
