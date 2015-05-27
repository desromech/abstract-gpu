#include "buffer.hpp"

inline GLenum mapBinding(agpu_buffer_binding_type binding)
{
    switch(binding)
    {
    case AGPU_ARRAY_BUFFER: return GL_ARRAY_BUFFER;
	case AGPU_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
	case AGPU_UNIFORM_BUFFER:  return GL_UNIFORM_BUFFER;
	case AGPU_DRAW_INDIRECT_BUFFER: return GL_DRAW_INDIRECT_BUFFER;
	default: abort();
	}
}

inline GLbitfield mapMappingFlags(agpu_buffer_mapping_flags flags)
{
    GLbitfield glflags = 0;
    
    if(flags & AGPU_MAP_READ_BIT)
        glflags |= GL_MAP_READ_BIT;
    if(flags & AGPU_MAP_WRITE_BIT)
        glflags |= GL_MAP_WRITE_BIT;
    if(flags & AGPU_MAP_PERSISTENT_BIT)
        glflags |= GL_MAP_PERSISTENT_BIT;
    if(flags & AGPU_MAP_COHERENT_BIT)
        glflags |= GL_MAP_COHERENT_BIT;
    if(flags & AGPU_MAP_DYNAMIC_STORAGE_BIT)
        glflags |= GL_DYNAMIC_STORAGE_BIT;

    return flags;
}

inline GLenum mapMappingAccess(agpu_mapping_access flags)
{
    switch(flags)
    {
    case AGPU_READ_ONLY: return GL_READ_ONLY;
    case AGPU_WRITE_ONLY: return GL_WRITE_ONLY;
    case AGPU_READ_WRITE: return GL_READ_WRITE;
    }
    
}

_agpu_buffer::_agpu_buffer()
{
    mappedPointer = nullptr;
}

void agpu_buffer::lostReferences()
{
    // Delete the buffer.
    device->glDeleteBuffers(1, &handle);
}

agpu_buffer *agpu_buffer::createBuffer(agpu_device *device, const agpu_buffer_description &description, agpu_pointer initialData)
{
    GLuint handle;
    auto binding = mapBinding(description.binding);
    auto mappingFlags = mapMappingFlags(description.mapping_flags);
    
    // Generate the buffer
    device->glGenBuffers(1, &handle);
    device->glBindBuffer(binding, handle);
    
    // Initialize the buffer storage
    device->glBufferStorage(binding, description.size, initialData, mappingFlags);
    
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
    if(mappedBuffer)
        return mappedBuffer;
    return mappedBuffer = glMapBuffer(target, mapMappingAccess(access);
}

agpu_error agpu_buffer::unmapBuffer()
{
    auto result = glUnmapBuffer(target);
    mappedBuffer = nullptr;
    
    return (result != GL_FALSE) ? AGPU_OK : AGPU_ERROR; 
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

