#ifndef _AGPU_GL_SHADER_HPP_
#define _AGPU_GL_SHADER_HPP_

#include <string>
#include <vector>
#include "device.hpp"

struct combine_texture_with_sampler
{
    unsigned int textureDescriptorSet;
    unsigned int textureDescriptorBinding;
    unsigned int textureUnit;

    unsigned int samplerDescriptorSet;
    unsigned int samplerDescriptorBinding;
    unsigned int samplerUnit;

    std::string name;
};

struct agpu_shader_forSignature: public Object<agpu_shader_forSignature>
{
public:
    agpu_shader_forSignature();

    void lostReferences();

    agpu_error compile(std::string *errorMessage);
    agpu_error attachToProgram(GLuint programHandle, std::string *errorMessage);

public:
    agpu_device *device;
    agpu_shader_type type;
    agpu_shader_language rawSourceLanguage;

    GLuint handle;
    std::string glslSource;
    std::vector<combine_texture_with_sampler> combinedTexturesWithSamplers;
};


struct _agpu_shader: public Object<_agpu_shader>
{
public:
    _agpu_shader();

    void lostReferences();

    static agpu_shader *createShader(agpu_device *device, agpu_shader_type type);
    agpu_error instanceForSignature(agpu_shader_signature *signature, agpu_shader_forSignature **result, std::string *errorMessage);

    agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
    agpu_error compileShader(agpu_cstring options);
    agpu_size getShaderCompilationLogLength();
    agpu_error getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer);

public:
    //agpu_error compileGLSLSource(bool parseAgpuPragmas, agpu_string sourceText, agpu_string_length sourceTextLength);

    agpu_device *device;
    agpu_shader_forSignature *genericShaderInstance;
    agpu_shader_type type;
    bool compiled;

    agpu_shader_language rawSourceLanguage;
    std::vector<uint8_t> rawShaderSource;

private:
    agpu_error getOrCreateGenericShaderInstance(agpu_shader_signature *signature, agpu_shader_forSignature **result, std::string *errorMessage);
    agpu_error getOrCreateSpirVShaderInstance(agpu_shader_signature *signature, agpu_shader_forSignature **result, std::string *errorMessage);
};


#endif //_AGPU_GL_SHADER_HPP_
