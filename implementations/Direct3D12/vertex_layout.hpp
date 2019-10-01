#ifndef AGPU_D3D12_VERTEX_LAYOUT_HPP
#define AGPU_D3D12_VERTEX_LAYOUT_HPP

#include <vector>
#include "device.hpp"

namespace AgpuD3D12
{

class ADXVertexLayout : public agpu::vertex_layout
{
public:
    ADXVertexLayout(const agpu::device_ref &cdevice);
    ~ADXVertexLayout();

    static agpu::vertex_layout_ref create(const agpu::device_ref &device);

    virtual agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes) override;

public:
    agpu::device_ref device;

    agpu_uint vertexBufferCount;
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
    std::vector<agpu_size> strides;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_VERTEX_LAYOUT_HPP
