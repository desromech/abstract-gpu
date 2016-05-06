#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "utility.hpp"
#include "buffer.hpp"
#include "texture_formats.hpp"

_agpu_vertex_binding::_agpu_vertex_binding()
{
    changed = true;
}

void _agpu_vertex_binding::lostReferences()
{
    device->onMainContextBlocking([&] {
        device->glBindVertexArray(0);
        device->glDeleteVertexArrays(1, &handle);
    });

    for(auto &vb : vertexBuffers)
    {
        if(vb)
            vb->release();
    }
    vertexLayout->release();
}

agpu_vertex_binding *_agpu_vertex_binding::createVertexBinding(agpu_device *device, agpu_vertex_layout *layout)
{
    GLuint handle;
    device->onMainContextBlocking([&] {
        device->glGenVertexArrays(1, &handle);
    });

	auto binding = new agpu_vertex_binding();
    binding->handle = handle;
	binding->device = device;
    binding->vertexLayout = layout;
    binding->vertexLayout->retain();
	return binding;
}

void _agpu_vertex_binding::bind()
{
    device->glBindVertexArray(handle);
    if(changed)
        updateBindings();
}

agpu_error _agpu_vertex_binding::bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers)
{
    if (count != vertexLayout->vertexBufferCount)
        return AGPU_ERROR;

    changed = true;
    for(size_t i = 0; i < count; ++i)
        vertex_buffers[i]->retain();

    for(auto &vb : vertexBuffers)
    {
        if(vb)
            vb->release();
    }

    vertexBuffers.resize(count);
    for(size_t i = 0; i < count; ++i)
        this->vertexBuffers[i] = vertex_buffers[i];

	return AGPU_OK;
}

agpu_error _agpu_vertex_binding::updateBindings()
{
    agpu_buffer *prevBuffer = nullptr;
    for (auto &attr : vertexLayout->attributes)
    {
        if (attr.buffer > vertexBuffers.size())
            return AGPU_ERROR;

        // Bind the buffer
        auto newBuffer = vertexBuffers[attr.buffer];
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
    auto isNormalized = isFormatNormalized(attribute.format);
    auto components = getFormatNumberOfComponents(attribute.format);
    auto type = mapExternalFormatType(attribute.format);
	device->glEnableVertexAttribArray(attribute.binding);
	device->glVertexAttribPointer(attribute.binding, components, type, isNormalized, (GLsizei)stride, reinterpret_cast<void*> (size_t(attribute.offset)));

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
