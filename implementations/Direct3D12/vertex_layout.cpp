#include "vertex_layout.hpp"

const int MaxVertexAttributes = 16;
const char * const VertexAttributeSemantics[] = {
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
};

inline DXGI_FORMAT mapImageFormat(agpu_field_type type, int components, bool normalized)
{
    switch (type)
    {
    case AGPU_FLOAT:
        switch (components)
        {
        case 1: return DXGI_FORMAT_R32_FLOAT;
        case 2: return DXGI_FORMAT_R32G32_FLOAT;
        case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    case AGPU_HALF_FLOAT:
        switch (components)
        {
        case 1: return DXGI_FORMAT_R16_FLOAT;
        case 2: return DXGI_FORMAT_R16G16_FLOAT;
        case 3: return DXGI_FORMAT_UNKNOWN;
        case 4: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        }
    case AGPU_DOUBLE: return DXGI_FORMAT_UNKNOWN;
    case AGPU_FIXED: return DXGI_FORMAT_UNKNOWN;
    case AGPU_BYTE:
        if (normalized)
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R8_SNORM;
            case 2: return DXGI_FORMAT_R8G8_SNORM;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R8G8B8A8_SNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R8_SINT;
            case 2: return DXGI_FORMAT_R8G8_SINT;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R8G8B8A8_SINT;
            }
        }
    case AGPU_UNSIGNED_BYTE:
        if (normalized)
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R8_UNORM;
            case 2: return DXGI_FORMAT_R8G8_UNORM;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R8G8B8A8_UNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R8_UINT;
            case 2: return DXGI_FORMAT_R8G8_UINT;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R8G8B8A8_UINT;
            }
        }
    case AGPU_SHORT:
        if (normalized)
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R16_SNORM;
            case 2: return DXGI_FORMAT_R16G16_SNORM;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R16G16B16A16_SNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R16_SINT;
            case 2: return DXGI_FORMAT_R16G16_SINT;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R16G16B16A16_SINT;
            }
        }
    case AGPU_UNSIGNED_SHORT:
        if (normalized)
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R16_UNORM;
            case 2: return DXGI_FORMAT_R16G16_UNORM;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R16G16B16A16_UNORM;
            }
        }
        else
        {
            switch (components)
            {
            case 1: return DXGI_FORMAT_R16_UINT;
            case 2: return DXGI_FORMAT_R16G16_UINT;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return DXGI_FORMAT_R16G16B16A16_UINT;
            }
        }
    case AGPU_INT:
        switch (components)
        {
        case 1: return DXGI_FORMAT_R32_SINT;
        case 2: return DXGI_FORMAT_R32G32_SINT;
        case 3: return DXGI_FORMAT_R32G32B32_SINT;
        case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
        }
    case AGPU_UNSIGNED_INT:
        switch (components)
        {
        case 1: return DXGI_FORMAT_R32_UINT;
        case 2: return DXGI_FORMAT_R32G32_UINT;
        case 3: return DXGI_FORMAT_R32G32B32_UINT;
        case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
        }
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

_agpu_vertex_layout::_agpu_vertex_layout()
{

}

void _agpu_vertex_layout::lostReferences()
{

}

_agpu_vertex_layout *_agpu_vertex_layout::create(agpu_device *device)
{
    auto layout = new agpu_vertex_layout();
    layout->device = device;
    return layout;
}

agpu_error _agpu_vertex_layout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(attributes);
    this->vertexBufferCount = vertex_buffer_count;
    inputElements.reserve(attribute_count);

    for (size_t i = 0; i < attribute_count; ++i)
    {
        agpu_vertex_attrib_description &attrib = attributes[i];
        if (attrib.binding >= MaxVertexAttributes)
            return AGPU_ERROR;

        D3D12_INPUT_ELEMENT_DESC element;
        element.SemanticName = VertexAttributeSemantics[attrib.binding];
        element.Format = mapImageFormat(attrib.type, attrib.components, attrib.normalized);
        element.InputSlot = attrib.buffer;
        element.AlignedByteOffset = (UINT)attrib.offset;
        element.InputSlotClass = attrib.divisor ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = attrib.divisor;

        for (size_t j = 0; j < attrib.rows; ++j)
        {
            element.SemanticIndex = (UINT)j;
            inputElements.push_back(element);
        }
    }

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
