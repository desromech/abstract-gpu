#ifndef _AGPU_GL_VERTEX_LAYOUT_HPP_
#define _AGPU_GL_VERTEX_LAYOUT_HPP_

#include <vector>
#include "device.hpp"

namespace AgpuGL
{

/**
* Vertex binding
*/
struct GLVertexLayout : public agpu::vertex_layout
{
public:
    GLVertexLayout();
    ~GLVertexLayout();

    static agpu::vertex_layout_ref createVertexLayout(const agpu::device_ref &device);

    virtual agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes) override;

public:
    agpu::device_ref device;

    agpu_uint vertexBufferCount;
    std::vector<agpu_vertex_attrib_description> attributes;
    std::vector<agpu_size> strides;
};

} // End of namespace AgpuGL

#endif //_AGPU_GL_VERTEX_LAYOUT_HPP_
