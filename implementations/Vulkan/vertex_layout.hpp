#ifndef AGPU_VERTEX_LAYOUT_HPP
#define AGPU_VERTEX_LAYOUT_HPP

#include "device.hpp"

struct _agpu_vertex_layout : public Object<_agpu_vertex_layout>
{
    struct VertexStructureDimensions
    {
        VertexStructureDimensions(agpu_uint size = 0, agpu_uint divisor = 0)
            : size(size), divisor(divisor) {}

        agpu_uint size;
        agpu_uint divisor;
    };

    _agpu_vertex_layout(agpu_device *device);
    void lostReferences();

    static _agpu_vertex_layout *create(agpu_device *device);

    agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size *vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);

    agpu_device *device;

    std::vector<VertexStructureDimensions> bufferDimensions;
    std::vector<agpu_vertex_attrib_description> allAttributes;
};

#endif //AGPU_VERTEX_LAYOUT_HPP