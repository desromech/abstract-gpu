#ifndef _AGPU_BUFFER_HPP_
#define _AGPU_BUFFER_HPP_

#include "device.hpp"

class _agpu_buffer: public Object<_agpu_buffer>
{
public:
    _agpu_buffer();

    void lostReferences();

    static agpu_buffer *createBuffer(agpu_device *device, const agpu_buffer_description &description, agpu_pointer initialData);
    
    agpu_pointer mapBuffer(agpu_mapping_access access);
    agpu_error unmapBuffer();
    agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data);
    
    void bind();
    
public:

    agpu_device *device;
    agpu_buffer_description description;
    GLenum target;
    GLuint handle;
    agpu_pointer mappedPointer;
};

#endif //_AGPU_BUFFER_HPP_
