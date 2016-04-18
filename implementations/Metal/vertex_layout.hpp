#ifndef AGPU_VERTEX_LAYOUT_HPP
#define AGPU_VERTEX_LAYOUT_HPP

#include "device.hpp"

struct _agpu_vertex_layout : public Object<_agpu_vertex_layout>
{
public:
    _agpu_vertex_layout(agpu_device *device);
    void lostReferences();

    static agpu_vertex_layout* create ( agpu_device* device );

    agpu_error addVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );

    agpu_device *device;
};

#endif //AGPU_VERTEX_LAYOUT_HPP
