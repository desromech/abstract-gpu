#ifndef AGPU_D3D12_VERTEX_BINDING_HPP
#define AGPU_D3D12_VERTEX_BINDING_HPP

#include <vector>
#include "device.hpp"

namespace AgpuD3D12
{

class ADXVertexBinding : public agpu::vertex_binding
{
public:
    ADXVertexBinding(const agpu::device_ref &cdevice, const agpu::vertex_layout_ref &layout);
    ~ADXVertexBinding();

    static agpu::vertex_binding_ref create(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout);

    virtual agpu_error bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers) override;
    virtual agpu_error bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size* offsets) override;

public:
    agpu::device_ref device;
    agpu::vertex_layout_ref layout;

    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
    std::vector<agpu::buffer_ref> vertexBuffers;

};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_VERTEX_BINDING_HPP
