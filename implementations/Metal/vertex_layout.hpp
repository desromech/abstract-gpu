#ifndef AGPU_VERTEX_LAYOUT_HPP
#define AGPU_VERTEX_LAYOUT_HPP

#include "device.hpp"
#include <vector>

namespace AgpuMetal
{
    
class AMtlVertexLayout : public agpu::vertex_layout
{
public:
    AMtlVertexLayout(const agpu::device_ref &device);
    ~AMtlVertexLayout();

    static agpu::vertex_layout_ref create(const agpu::device_ref &device);

    agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);
    
    MTLVertexDescriptor *createVertexDescriptor(agpu_uint vertexBufferStartIndex);
    
    agpu::device_ref device;
    std::vector<agpu_vertex_attrib_description> vertexAttributes;
    std::vector<agpu_size> vertexStrides;
};

} // End of namespace AgpuMetal

#endif //AGPU_VERTEX_LAYOUT_HPP
