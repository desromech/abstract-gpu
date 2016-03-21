#ifndef AGPU_D3D12_VERTEX_LAYOUT_HPP
#define AGPU_D3D12_VERTEX_LAYOUT_HPP

#include <vector>
#include "device.hpp"

struct _agpu_vertex_layout : public Object<_agpu_vertex_layout>
{
public:
    _agpu_vertex_layout();

    void lostReferences();

    static _agpu_vertex_layout *create(agpu_device *device);

    agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);

public:
    agpu_device *device;

    agpu_uint vertexBufferCount;
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
};

#endif //AGPU_D3D12_VERTEX_LAYOUT_HPP
