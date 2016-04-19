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

agpu_error _agpu_vertex_layout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(vertex_strides);
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
        element.Format = (DXGI_FORMAT)attrib.format;
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

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings(agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(vertex_layout);
    return vertex_layout->addVertexAttributeBindings(vertex_buffer_count, vertex_strides, attribute_count, attributes);
}
