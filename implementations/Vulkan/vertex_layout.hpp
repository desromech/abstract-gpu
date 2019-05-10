#ifndef AGPU_VERTEX_LAYOUT_HPP
#define AGPU_VERTEX_LAYOUT_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkVertexLayout : public agpu::vertex_layout
{
public:
    struct VertexStructureDimensions
    {
        VertexStructureDimensions(agpu_uint size = 0, agpu_uint divisor = 0)
            : size(size), divisor(divisor) {}

        agpu_uint size;
        agpu_uint divisor;
    };

    AVkVertexLayout(const agpu::device_ref &device);
    ~AVkVertexLayout();

    static agpu::vertex_layout_ref create(const agpu::device_ref &device);

    agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);

    agpu::device_ref device;

    std::vector<VertexStructureDimensions> bufferDimensions;
    std::vector<agpu_vertex_attrib_description> allAttributes;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VERTEX_LAYOUT_HPP
