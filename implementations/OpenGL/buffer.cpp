#include "buffer.hpp"

inline GLenum mapBinding(agpu_buffer_binding_type binding)
{
    switch(binding)
    {
    case AGPU_ARRAY_BUFFER: return GL_ARRAY_BUFFER;
	case AGPU_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
	case AGPU_UNIFORM_BUFFER:  return GL_UNIFORM_BUFFER;
	case AGPU_STORAGE_BUFFER:  return GL_SHADER_STORAGE_BUFFER;
	case AGPU_DRAW_INDIRECT_BUFFER: return GL_DRAW_INDIRECT_BUFFER;
	default: abort();
	}
}

_agpu_buffer::_agpu_buffer()
{
    mappedPointer = nullptr;
}

void agpu_buffer::lostReferences()
{
    device->onMainContextBlocking([&]{
        // Delete the buffer.
        device->glDeleteBuffers(1, &handle);
    });
}

agpu_buffer *agpu_buffer::createBuffer(agpu_device *device, const agpu_buffer_description &description, agpu_pointer initialData)
{
    // A device is needed.
    if(!device)
        return nullptr;

    GLuint handle;
    auto binding = mapBinding(description.binding);
    auto mappingFlags = mapMappingFlags(description.mapping_flags);

    device->onMainContextBlocking([&]{
        // Generate the buffer
        device->glGenBuffers(1, &handle);
        device->glBindBuffer(binding, handle);

        // Initialize the buffer storage
        device->glBufferStorage(binding, description.size, initialData, mappingFlags);
    });

    // Create the buffer object.
    auto buffer = new agpu_buffer();
    buffer->device = device;
    buffer->description = description;
    buffer->target = binding;
    buffer->handle = handle;

    return buffer;
}


agpu_pointer agpu_buffer::mapBuffer(agpu_mapping_access access)
{
    if(mappedPointer)
        return mappedPointer;

    device->onMainContextBlocking([&]{
        bind();
        mappedPointer = device->glMapBuffer(target, mapMappingAccess(access));
    });
    return mappedPointer;
}

agpu_error agpu_buffer::unmapBuffer()
{
    GLenum result;
    device->onMainContextBlocking([&]{
        bind();
        result = device->glUnmapBuffer(target);
        mappedPointer = nullptr;
    });

    return (result != GL_FALSE) ? AGPU_OK : AGPU_ERROR;
}

agpu_error agpu_buffer::uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    device->onMainContextBlocking([&]{
        bind();
        device->glBufferSubData(target, offset, size, data);
    });
    return AGPU_OK;
}

agpu_error agpu_buffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    device->onMainContextBlocking([&]{
        bind();
        device->glGetBufferSubData(target, offset, size, data);
    });
    return AGPU_OK;
}

agpu_error agpu_buffer::flushWholeBuffer ()
{
    return AGPU_OK;
}

agpu_error agpu_buffer::invalidateWholeBuffer ()
{
    return AGPU_OK;
}

void agpu_buffer::bind()
{
    device->glBindBuffer(target, handle);
}

// C Interface

AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->retain();
}

AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->release();
}

AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description )
{
    CHECK_POINTER(buffer);
    CHECK_POINTER(description);
    *description = buffer->description;
    return AGPU_OK;
}

AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags )

{
    if(!buffer) return nullptr;
    return buffer->mapBuffer(flags);
}

AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->unmapBuffer();
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    return buffer->uploadBufferData(offset, size, data);
}

AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    return buffer->readBufferData(offset, size, data);
}


AGPU_EXPORT agpu_error agpuFlushWholeBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->flushWholeBuffer();
}

AGPU_EXPORT agpu_error agpuInvalidateWholeBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->invalidateWholeBuffer();
}
