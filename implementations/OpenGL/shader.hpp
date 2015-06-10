#ifndef _AGPU_GL_SHADER_HPP_
#define _AGPU_GL_SHADER_HPP_

#include "device.hpp"

class _agpu_shader: public Object<_agpu_shader>
{
public:
    _agpu_shader();

    void lostReferences();

    static agpu_shader *createShader(agpu_device *device, agpu_shader_type type);
    
    agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
    agpu_error compileShader(agpu_cstring options);
    agpu_size getShaderCompilationLogLength();
    agpu_error getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer);

public:
    agpu_device *device;
    agpu_shader_type type;
    GLuint handle;
    bool compiled;
};


#endif //_AGPU_GL_SHADER_HPP_