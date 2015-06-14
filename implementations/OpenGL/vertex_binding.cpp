#include "vertex_binding.hpp"
#include "utility.hpp"
#include "buffer.hpp"

_agpu_vertex_binding::_agpu_vertex_binding()
{
	
}

void _agpu_vertex_binding::lostReferences()
{
	device->glBindVertexArray(0);
	device->glDeleteVertexArrays(1, &handle);
}

agpu_vertex_binding *_agpu_vertex_binding::createVertexBinding(agpu_device *device)
{
	auto binding = new agpu_vertex_binding();
	binding->device = device;
	device->glGenVertexArrays(1, &binding->handle);
	return binding;
}

void _agpu_vertex_binding::bind()
{
	device->glBindVertexArray(handle);
}

agpu_error _agpu_vertex_binding::addVertexBufferBindings ( agpu_buffer* vertex_buffer, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
	bind();
	vertex_buffer->bind();
	return activateVertexAttributes(vertex_buffer->description.stride, attribute_count, attributes);
}

agpu_error _agpu_vertex_binding::activateVertexAttributes ( agpu_size stride, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
	for(agpu_size i = 0; i < attribute_count; ++i)
	{
		auto error = activateVertexAttribute(stride, attributes[i]);
		if(error < 0)
			return error;
	}
	
	return AGPU_OK;
}

agpu_error _agpu_vertex_binding::activateVertexAttribute ( agpu_size stride, agpu_vertex_attrib_description &attribute )
{
	GLenum type = mapFieldType(attribute.type);
	device->glEnableVertexAttribArray(attribute.binding);
	device->glVertexAttribPointer(attribute.binding, attribute.components, type, attribute.normalized, stride, reinterpret_cast<void*> (size_t(attribute.offset)));
	
	return AGPU_OK;
}

// C interface
AGPU_EXPORT agpu_error agpuAddVertexBindingReference ( agpu_vertex_binding* vertex_binding )
{
	CHECK_POINTER(vertex_binding);
	vertex_binding->retain();
	return AGPU_OK;
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding ( agpu_vertex_binding* vertex_binding )
{
	CHECK_POINTER(vertex_binding);
	vertex_binding->release();
	return AGPU_OK;
}

AGPU_EXPORT agpu_error agpuAddVertexBufferBindings ( agpu_vertex_binding* vertex_binding, agpu_buffer* vertex_buffer, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
	CHECK_POINTER(vertex_binding);
	CHECK_POINTER(vertex_buffer);
	CHECK_POINTER(attributes);
	return vertex_binding->addVertexBufferBindings(vertex_buffer, attribute_count, attributes);
}
