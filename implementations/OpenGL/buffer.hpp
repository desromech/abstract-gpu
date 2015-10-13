#ifndef _AGPU_BUFFER_HPP_
#define _AGPU_BUFFER_HPP_

#include "device.hpp"

inline GLbitfield mapMappingFlags(agpu_bitfield flags)
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

    return glflags;
}

inline GLenum mapMappingAccess(agpu_mapping_access flags)
{
    switch(flags)
    {
    case AGPU_READ_ONLY: return GL_READ_ONLY;
    case AGPU_WRITE_ONLY: return GL_WRITE_ONLY;
    case AGPU_READ_WRITE: return GL_READ_WRITE;
    default: abort();
    }

}


struct _agpu_buffer: public Object<_agpu_buffer>
{
public:
    _agpu_buffer();

    void lostReferences();

    static agpu_buffer *createBuffer(agpu_device *device, const agpu_buffer_description &description, agpu_pointer initialData);

    agpu_pointer mapBuffer(agpu_mapping_access access);
    agpu_error unmapBuffer();
    agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data);
    agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data);

    void bind();

public:

    agpu_device *device;
    agpu_buffer_description description;
    GLenum target;
    GLuint handle;
    agpu_pointer mappedPointer;
};

#endif //_AGPU_BUFFER_HPP_
