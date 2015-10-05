#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "utility.hpp"
#include "buffer.hpp"

_agpu_vertex_binding::_agpu_vertex_binding()
    : dirtyCount(0)
{

}

void _agpu_vertex_binding::lostReferences()
{
    for(auto &vb : vertexBuffers)
    {
        if(vb)
            vb->release();
    }
    vertexLayout->release();
}

agpu_vertex_binding *_agpu_vertex_binding::createVertexBinding(agpu_device *device, agpu_vertex_layout *layout)
{
	auto binding = new agpu_vertex_binding();
	binding->device = device;
    binding->vertexLayout = layout;
    binding->vertexLayout->retain();
	return binding;
}

void _agpu_vertex_binding::bind()
{
    auto context = OpenGLContext::getCurrent();
    auto &allVaos = context->vertexArrayObjects;
    auto it = allVaos.find(this);
    if(it != allVaos.end())
    {
        auto &data = it->second;
        device->glBindVertexArray(data.first);
        if(data.second != dirtyCount)
        {
            updateBindings();
            data.second = dirtyCount;
        }
    }
    else
    {
        GLuint handle;
        device->glGenVertexArrays(1, &handle);
        allVaos.insert(std::make_pair(this, std::make_pair(handle, dirtyCount)));
        device->glBindVertexArray(handle);
        updateBindings();
    }
}

agpu_error _agpu_vertex_binding::bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers)
{
    if (count != vertexLayout->vertexBufferCount)
        return AGPU_ERROR;

    ++dirtyCount;
    for(auto &vb : vertexBuffers)
    {
        if(vb)
            vb->release();
    }

    vertexBuffers.resize(count);
    for(size_t i = 0; i < count; ++i)
    {
        vertex_buffers[i]->retain();
        vertexBuffers[i] = vertex_buffers[i];
    }

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
