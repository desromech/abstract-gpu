#ifndef _AGPU_BUFFER_HPP_
#define _AGPU_BUFFER_HPP_

#include "device.hpp"

namespace AgpuGL
{

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

inline GLenum mapBufferRangeMappingAccess(agpu_mapping_access flags)
{
    switch(flags)
    {
    case AGPU_READ_ONLY: return GL_MAP_READ_BIT;
    case AGPU_WRITE_ONLY: return GL_MAP_WRITE_BIT;
    case AGPU_READ_WRITE: return GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
    default: abort();
    }

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


struct GLBuffer: public agpu::buffer
{
public:
    GLBuffer();
    ~GLBuffer();

    static agpu::buffer_ref createBuffer(const agpu::device_ref &device, const agpu_buffer_description &description, agpu_pointer initialData);

    void dumpToFile(const char *fileName);

    virtual agpu_pointer mapBuffer(agpu_mapping_access access) override;
    virtual agpu_error unmapBuffer() override;
	virtual agpu_error getDescription(agpu_buffer_description* description) override;
    virtual agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;
    virtual agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;
    virtual agpu_error flushWholeBuffer () override;
    virtual agpu_error invalidateWholeBuffer () override;

    void bind();

public:

    agpu::device_ref device;
    agpu_buffer_description description;
    GLenum target;
    GLuint handle;
    GLbitfield extraMappingFlags;
    agpu_pointer mappedPointer;
};

} // End of namespace AgpuGL

#endif //_AGPU_BUFFER_HPP_
