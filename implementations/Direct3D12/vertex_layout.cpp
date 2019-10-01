#include "vertex_layout.hpp"

namespace AgpuD3D12
{

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

ADXVertexLayout::ADXVertexLayout(const agpu::device_ref &cdevice)
    : device(cdevice)
{

}

ADXVertexLayout::~ADXVertexLayout()
{

}

agpu::vertex_layout_ref ADXVertexLayout::create(const agpu::device_ref &device)
{
    return agpu::makeObject<ADXVertexLayout> (device);
}

agpu_error ADXVertexLayout::addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
    CHECK_POINTER(vertex_strides);
    CHECK_POINTER(attributes);
    this->vertexBufferCount = vertex_buffer_count;
    inputElements.reserve(attribute_count);
	strides.reserve(vertex_buffer_count);
	for (size_t i = 0; i < vertex_buffer_count; ++i)
		strides.push_back(vertex_strides[i]);

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
        element.InputSlotClass = attrib.divisor != 0 ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = attrib.divisor;

        for (size_t j = 0; j < attrib.rows; ++j)
        {
            element.SemanticIndex = (UINT)j;
            inputElements.push_back(element);
        }
    }

    return AGPU_OK;
}

} // End of namespace AgpuD3D12
