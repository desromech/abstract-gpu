#ifndef _AGPU_GL_VERTEX_BINDING_HPP_
#define _AGPU_GL_VERTEX_BINDING_HPP_

#include "device.hpp"

/**
 * Vertex binding
 */
class _agpu_vertex_binding: public Object<_agpu_vertex_binding>
{
public:
	_agpu_vertex_binding();
	
    void lostReferences();
	
    static agpu_vertex_binding *createVertexBinding(agpu_device *device);

	agpu_error addVertexBufferBindings ( agpu_buffer* vertex_buffer, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );
	
	agpu_error activateVertexAttributes ( agpu_size stride, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );
	agpu_error activateVertexAttribute ( agpu_size stride, agpu_vertex_attrib_description &attribute );
	
	void bind();
	
public:
	agpu_device *device;
	GLuint handle;
};

#endif //_AGPU_GL_VERTEX_BINDING_HPP_