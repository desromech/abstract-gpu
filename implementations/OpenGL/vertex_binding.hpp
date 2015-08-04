#ifndef _AGPU_GL_VERTEX_BINDING_HPP_
#define _AGPU_GL_VERTEX_BINDING_HPP_

#include "device.hpp"

/**
 * Vertex binding
 */
struct _agpu_vertex_binding: public Object<_agpu_vertex_binding>
{
public:
	_agpu_vertex_binding();
	
    void lostReferences();
	
    static agpu_vertex_binding *createVertexBinding(agpu_device *device, agpu_vertex_layout *layout);

    agpu_error bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers);
	
	agpu_error activateVertexAttribute ( agpu_size stride, agpu_vertex_attrib_description &attribute );
	
	void bind();
	
public:
	agpu_device *device;
    agpu_vertex_layout *vertexLayout;
	GLuint handle;
};

#endif //_AGPU_GL_VERTEX_BINDING_HPP_