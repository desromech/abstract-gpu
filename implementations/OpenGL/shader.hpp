#ifndef _AGPU_GL_SHADER_HPP_
#define _AGPU_GL_SHADER_HPP_

#include <string>
#include <vector>
#include "device.hpp"

struct LocationBinding
{
    LocationBinding() {}
    LocationBinding(const std::string &name, GLint location)
        : location(location), name(name) {}

    GLint location;
    std::string name;
};

struct _agpu_shader: public Object<_agpu_shader>
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
    std::vector<LocationBinding> attributeBindings;
    std::vector<LocationBinding> uniformBindings;
    std::vector<LocationBinding> samplerBindings;

private:
    void parseShaderPragmas(agpu_string sourceTextBegin, agpu_string sourceTextEnd);
    void processPragma(std::vector<std::string> &pragma);
};


#endif //_AGPU_GL_SHADER_HPP_
