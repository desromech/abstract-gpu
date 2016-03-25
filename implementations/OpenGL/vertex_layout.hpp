#ifndef _AGPU_GL_VERTEX_LAYOUT_HPP_
#define _AGPU_GL_VERTEX_LAYOUT_HPP_

#include <vector>
#include "device.hpp"

/**
* Vertex binding
*/
struct _agpu_vertex_layout : public Object<_agpu_vertex_layout>
{
public:
    _agpu_vertex_layout();

    void lostReferences();

    static agpu_vertex_layout *createVertexLayout(agpu_device *device);

    agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);

public:
    agpu_device *device;
    agpu_vertex_layout *vertexLayout;

    agpu_uint vertexBufferCount;
    std::vector<agpu_vertex_attrib_description> attributes;
};

#endif //_AGPU_GL_VERTEX_LAYOUT_HPP_