#ifndef AGPU_METAL_VERTEX_BINDING_HPP
#define AGPU_METAL_VERTEX_BINDING_HPP

#include "device.hpp"
#include <vector>

namespace AgpuMetal
{
    
struct AMtlVertexBinding : public agpu::vertex_binding
{
public:
    AMtlVertexBinding(const agpu::device_ref &device);
    ~AMtlVertexBinding();

    static agpu::vertex_binding_ref create(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout);

    virtual agpu_error bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers) override;
	virtual agpu_error bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size* offsets) override;

    agpu::device_ref device;
    std::vector<agpu::buffer_ref> buffers;
    std::vector<agpu_size> offsets;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_VERTEX_BINDING_HPP
