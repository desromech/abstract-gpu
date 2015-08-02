#ifndef _AGPU_GL_PROGRAM_HPP_
#define _AGPU_GL_PROGRAM_HPP_

#include "device.hpp"

struct _agpu_program: public Object<_agpu_shader>
{
public:
    _agpu_program();

    void lostReferences();

    static agpu_program *createProgram(agpu_device *device);
    
    agpu_error attachShader(agpu_shader* shader);
    agpu_error linkProgram();
    agpu_size getProgramLinkingLogLength();
    agpu_error getProgramLinkingLog(agpu_size buffer_size, agpu_string_buffer buffer);
    
    agpu_error bindAttributeLocation(agpu_cstring name, agpu_int location);

    agpu_int getUniformLocation ( agpu_cstring name );
        
public:
    agpu_device *device;
    GLuint handle;
    bool linked;
};


#endif //_AGPU_GL_PROGRAM_HPP_