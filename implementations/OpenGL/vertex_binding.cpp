#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "utility.hpp"
#include "buffer.hpp"

_agpu_vertex_binding::_agpu_vertex_binding()
{
	
}

void _agpu_vertex_binding::lostReferences()
{
	device->glBindVertexArray(0);
	device->glDeleteVertexArrays(1, &handle);
    vertexLayout->release();
}

agpu_vertex_binding *_agpu_vertex_binding::createVertexBinding(agpu_device *device, agpu_vertex_layout *layout)
{
	auto binding = new agpu_vertex_binding();
	binding->device = device;
    binding->vertexLayout = layout;
    binding->vertexLayout->retain();
	device->glGenVertexArrays(1, &binding->handle);
	return binding;
}

void _agpu_vertex_binding::bind()
{
	device->glBindVertexArray(handle);
}

agpu_error _agpu_vertex_binding::bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers)
{
	bind();
    if (count != vertexLayout->vertexBufferCount)
        return AGPU_ERROR;

    agpu_buffer *prevBuffer = nullptr;
    for (auto &attr : vertexLayout->attributes)
    {
        if (attr.buffer > count)
            return AGPU_ERROR;

        // Bind the buffer
        auto newBuffer = vertex_buffers[attr.buffer];
        if (newBuffer != prevBuffer)
            newBuffer->bind();

        // Activate the attribute
        auto error = activateVertexAttribute(newBuffer->description.stride, attr);
        if (error < 0)
            return error;
    }

	return AGPU_OK;
}

agpu_error _agpu_vertex_binding::activateVertexAttribute ( agpu_size stride, agpu_vertex_attrib_description &attribute )
{
	GLenum type = mapFieldType(attribute.type);
	device->glEnableVertexAttribArray(attribute.binding);
	device->glVertexAttribPointer(attribute.binding, attribute.components, type, attribute.normalized, (GLsizei)stride, reinterpret_cast<void*> (size_t(attribute.offset)));
	
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

AGPU_EXPORT agpu_error agpuBindVertexBuffers(agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers)
{
    CHECK_POINTER(vertex_binding);
    return vertex_binding->bindVertexBuffers(count, vertex_buffers);
}
