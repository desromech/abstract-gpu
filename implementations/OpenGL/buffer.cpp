#include "buffer.hpp"

namespace AgpuGL
{

inline GLenum mapBinding(agpu_buffer_binding_type binding)
{
    switch(binding)
    {
    case AGPU_ARRAY_BUFFER: return GL_ARRAY_BUFFER;
	case AGPU_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
	case AGPU_UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
	case AGPU_STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER;
	case AGPU_DRAW_INDIRECT_BUFFER: return GL_DRAW_INDIRECT_BUFFER;
	default: abort();
	}
}

GLBuffer::GLBuffer()
{
    mappedPointer = nullptr;
}


GLBuffer::~GLBuffer()
{
    device.as<GLDevice> ()->onMainContextBlocking([&]{
        // Delete the buffer.
        device.as<GLDevice> ()->glDeleteBuffers(1, &handle);
    });
}

agpu::buffer_ref GLBuffer::createBuffer(const agpu::device_ref &device, const agpu_buffer_description &description, agpu_pointer initialData)
{
    // A device is needed.
    if(!device)
        return agpu::buffer_ref();

    GLuint handle;
    auto binding = mapBinding(description.binding);
    auto mappingFlags = mapMappingFlags(description.mapping_flags);

    deviceForGL->onMainContextBlocking([&]{
        // Generate the buffer
        deviceForGL->glGenBuffers(1, &handle);
        deviceForGL->glBindBuffer(binding, handle);

        // Initialize the buffer storage
        //printf("createBuffer %d %p\n", int(description.size), initialData);
        deviceForGL->glBufferStorage(binding, description.size, initialData, mappingFlags);
    });

    // Create the buffer object.
    auto result = agpu::makeObject<GLBuffer> ();
    auto buffer = result.as<GLBuffer> ();
    buffer->device = device;
    buffer->description = description;
    buffer->target = binding;
    buffer->handle = handle;
    buffer->extraMappingFlags = 0;
    if(description.mapping_flags & AGPU_MAP_PERSISTENT_BIT)
    {
        buffer->extraMappingFlags |= GL_MAP_PERSISTENT_BIT;
        if(description.mapping_flags & AGPU_MAP_COHERENT_BIT)
            buffer->extraMappingFlags |= GL_MAP_COHERENT_BIT;
        else
            buffer->extraMappingFlags |= GL_MAP_FLUSH_EXPLICIT_BIT;
    }

    return result;
}


agpu_pointer GLBuffer::mapBuffer(agpu_mapping_access access)
{
    if(mappedPointer)
        return mappedPointer;

    deviceForGL->onMainContextBlocking([&]{
        bind();
        if(extraMappingFlags != 0)
            mappedPointer = deviceForGL->glMapBufferRange(target, 0, description.size, mapBufferRangeMappingAccess(access) | extraMappingFlags);
        else
            mappedPointer = deviceForGL->glMapBuffer(target, mapMappingAccess(access));
    });
    return mappedPointer;
}

agpu_error GLBuffer::unmapBuffer()
{
    GLenum result;
    deviceForGL->onMainContextBlocking([&]{
        bind();
        result = deviceForGL->glUnmapBuffer(target);
        mappedPointer = nullptr;
    });

    return (result != GL_FALSE) ? AGPU_OK : AGPU_ERROR;
}

agpu_error GLBuffer::getDescription(agpu_buffer_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

void GLBuffer::dumpToFile(const char *fileName)
{
    if(!mappedPointer && extraMappingFlags == 0) return;

	FILE *f;
#ifdef _WIN32
	auto error = fopen_s(&f, fileName, "wb");
	if (error) return;
#else
	f = fopen(fileName, "wb");
#endif
    auto res = fwrite(mappedPointer, description.size, 1, f);
    (void)res;
    fclose(f);
}

agpu_error GLBuffer::uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    //printf("uploadBufferData %d %d %p\n", int(offset), int(size), data);
    deviceForGL->onMainContextBlocking([&]{
        bind();
        deviceForGL->glBufferSubData(target, offset, size, data);
    });
    return AGPU_OK;
}

agpu_error GLBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    deviceForGL->onMainContextBlocking([&]{
        bind();
        deviceForGL->glGetBufferSubData(target, offset, size, data);
    });
    return AGPU_OK;
}

agpu_error GLBuffer::flushWholeBuffer ()
{
    if((extraMappingFlags & GL_MAP_FLUSH_EXPLICIT_BIT) == 0)
        return AGPU_OK;

    deviceForGL->onMainContextBlocking([&]{
        bind();
        deviceForGL->glFlushMappedBufferRange(target, 0, description.size);
    });
    return AGPU_OK;
}

agpu_error GLBuffer::invalidateWholeBuffer ()
{
    return AGPU_OK;
}

void GLBuffer::bind()
{
    deviceForGL->glBindBuffer(target, handle);
}

} // End of namespace AgpuGL
