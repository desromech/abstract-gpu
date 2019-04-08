#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "utility.hpp"
#include "buffer.hpp"
#include "texture_formats.hpp"

namespace AgpuGL
{

GLVertexBinding::GLVertexBinding()
{
    changed = true;
}

GLVertexBinding::~GLVertexBinding()
{
    deviceForGL->onMainContextBlocking([&] {
        deviceForGL->glBindVertexArray(0);
        deviceForGL->glDeleteVertexArrays(1, &handle);
    });
}

agpu::vertex_binding_ref GLVertexBinding::createVertexBinding(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout)
{
    GLuint handle;
    deviceForGL->onMainContextBlocking([&] {
        deviceForGL->glGenVertexArrays(1, &handle);
    });

    auto result = agpu::makeObject<GLVertexBinding> ();
	auto binding = result.as<GLVertexBinding> ();
    binding->handle = handle;
	binding->device = device;
    binding->vertexLayout = layout;
	return result;
}

void GLVertexBinding::bind()
{
    deviceForGL->glBindVertexArray(handle);
    if(changed)
        updateBindings();
}

agpu_error GLVertexBinding::bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers)
{
    return bindVertexBuffersWithOffsets(count, vertex_buffers, nullptr);
}

agpu_error GLVertexBinding::bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size *offsets)
{
    if (count != vertexLayout.as<GLVertexLayout> ()->vertexBufferCount)
        return AGPU_ERROR;

    changed = true;
    this->vertexBuffers.resize(count);
    this->offsets.resize(count);
    for(size_t i = 0; i < count; ++i)
    {
        this->vertexBuffers[i] = vertex_buffers[i];
        this->offsets[i] = offsets ? offsets[i] : 0;
    }

	return AGPU_OK;
}

agpu_error GLVertexBinding::updateBindings()
{
    agpu::buffer_ref prevBuffer;
    auto glVertxLayout = vertexLayout.as<GLVertexLayout> ();
    for (auto &attr : glVertxLayout->attributes)
    {
        if (attr.buffer > vertexBuffers.size())
            return AGPU_ERROR;

        // Bind the buffer
        auto newBuffer = vertexBuffers[attr.buffer];
        if (newBuffer != prevBuffer)
        {
            newBuffer.as<GLBuffer>()->bind();
            prevBuffer = newBuffer;
        }

        // Activate the attribute
        auto error = activateVertexAttribute(glVertxLayout->strides[attr.buffer], attr, offsets[attr.buffer]);
        if (error < 0)
            return error;
    }

    return AGPU_OK;
}

agpu_error GLVertexBinding::activateVertexAttribute ( agpu_size stride, agpu_vertex_attrib_description &attribute, agpu_size bufferOffset )
{
    auto isNormalized = isFormatNormalized(attribute.format);
    auto components = getFormatNumberOfComponents(attribute.format);
    auto type = mapExternalFormatType(attribute.format);
	deviceForGL->glEnableVertexAttribArray(attribute.binding);
    if(isIntegerVertexAttributeFormat(attribute.format))
        deviceForGL->glVertexAttribIPointer(attribute.binding, components, type, (GLsizei)stride, reinterpret_cast<void*> (size_t(attribute.offset + bufferOffset)));
    else
	   deviceForGL->glVertexAttribPointer(attribute.binding, components, type, isNormalized, (GLsizei)stride, reinterpret_cast<void*> (size_t(attribute.offset + bufferOffset)));

	return AGPU_OK;
}

} // End of namespace AgpuGL
