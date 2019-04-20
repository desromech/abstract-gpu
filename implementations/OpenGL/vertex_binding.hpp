#ifndef _AGPU_GL_VERTEX_BINDING_HPP_
#define _AGPU_GL_VERTEX_BINDING_HPP_

#include "device.hpp"
#include <vector>

namespace AgpuGL
{

/**
 * Vertex binding
 */
struct GLVertexBinding: public agpu::vertex_binding
{
public:
	GLVertexBinding();
	~GLVertexBinding();

    static agpu::vertex_binding_ref createVertexBinding(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout);

    agpu_error bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers);
	agpu_error bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size *offsets);

	agpu_error activateVertexAttribute ( agpu_size stride, agpu_vertex_attrib_description &attribute, agpu_size bufferOffset );

public:
	agpu::device_ref device;
    agpu::vertex_layout_ref vertexLayout;
    std::vector<agpu::buffer_ref> vertexBuffers;
	std::vector<agpu_size> offsets;

    void bind();
    agpu_error updateBindings();
    GLuint handle;
    bool changed;
};

} // End of namespace AgpuGL

#endif //_AGPU_GL_VERTEX_BINDING_HPP_
