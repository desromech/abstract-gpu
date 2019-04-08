#ifndef _AGPU_GL_SHADER_HPP_
#define _AGPU_GL_SHADER_HPP_

#include "device.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>

namespace AgpuGL
{

struct TextureWithSamplerCombination
{
    unsigned int textureDescriptorSet;
    unsigned int textureDescriptorBinding;

    unsigned int samplerDescriptorSet;
    unsigned int samplerDescriptorBinding;

    bool operator==(const TextureWithSamplerCombination &o) const
    {
        return textureDescriptorSet == o.textureDescriptorSet &&
            textureDescriptorBinding == o.textureDescriptorBinding &&
            samplerDescriptorSet == o.samplerDescriptorSet &&
            samplerDescriptorBinding == o.samplerDescriptorBinding;
    }

    bool operator<(const TextureWithSamplerCombination &o) const
    {
        if(textureDescriptorSet == o.textureDescriptorSet)
        {
            if(textureDescriptorBinding == o.textureDescriptorBinding)
            {
                if(samplerDescriptorSet == o.samplerDescriptorSet)
                {
                    return samplerDescriptorBinding < o.samplerDescriptorBinding;
                }

                return samplerDescriptorSet < o.samplerDescriptorSet;
            }

            return textureDescriptorBinding < o.textureDescriptorBinding;
        }

        return textureDescriptorSet < o.textureDescriptorSet;
    }

    size_t hash() const
    {
        return std::hash<unsigned int> ()(textureDescriptorSet) ^
            std::hash<unsigned int> ()(textureDescriptorBinding) ^
            std::hash<unsigned int> ()(samplerDescriptorSet) ^
            std::hash<unsigned int> ()(samplerDescriptorBinding);
    }

    std::string createName() const
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "SpirV_CombinedSampledImage_%d_%d_WithSampler_%d_%d",
			textureDescriptorSet, textureDescriptorBinding,
			samplerDescriptorSet, samplerDescriptorBinding);
        return buffer;
    }
};

} // End of namespace AgpuGL

namespace std
{
template<>
struct hash<AgpuGL::TextureWithSamplerCombination>
{
    size_t operator()(const AgpuGL::TextureWithSamplerCombination &combination) const
    {
        return combination.hash();
    }
};
}

namespace AgpuGL
{

struct MappedTextureWithSamplerCombination
{
    TextureWithSamplerCombination combination;

    unsigned int sourceTextureUnit;
    unsigned int sourceSamplerUnit;

    unsigned int mappedTextureUnit;
    unsigned int mappedSamplerUnit;

    std::string name;
};

typedef std::unordered_map<TextureWithSamplerCombination, MappedTextureWithSamplerCombination> TextureWithSamplerCombinationMap;

struct GLShaderForSignature: public agpu::base_interface
{
public:
	typedef GLShaderForSignature main_interface;

    GLShaderForSignature();
    ~GLShaderForSignature();

    agpu_error compile(std::string *errorMessage);
    agpu_error attachToProgram(GLuint programHandle, std::string *errorMessage);

public:
    agpu::device_ref device;
    agpu_shader_type type;
    agpu_shader_language rawSourceLanguage;

    GLuint handle;
    std::string glslSource;
};

typedef agpu::ref<GLShaderForSignature> GLShaderForSignatureRef;

struct GLShader: public agpu::shader
{
public:
    GLShader();
    ~GLShader();

    static agpu::shader_ref createShader(const agpu::device_ref &device, agpu_shader_type type);
    agpu_error instanceForSignature(const agpu::shader_signature_ref &signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, const std::string &entryPoint, GLShaderForSignatureRef *result, std::string *errorMessage);

    virtual agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength) override;
    virtual agpu_error compileShader(agpu_cstring options) override;
    virtual agpu_size getCompilationLogLength() override;
    virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) override;

    std::vector<TextureWithSamplerCombination> &getTextureWithSamplerCombination(const std::string &entryPointName);

public:
    //agpu_error compileGLSLSource(bool parseAgpuPragmas, agpu_string sourceText, agpu_string_length sourceTextLength);

    agpu::device_ref device;
    GLShaderForSignatureRef genericShaderInstance;
    agpu_shader_type type;
    bool compiled;

    agpu_shader_language rawSourceLanguage;
    std::vector<uint8_t> rawShaderSource;

private:
    void extractGenericShaderTextureWithSamplerCombinations(const std::string &entryPointName);
    void extractSpirVTextureWithSamplerCombinations(const std::string &entryPointName);

    agpu_error getOrCreateGenericShaderInstance(const agpu::shader_signature_ref &signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, GLShaderForSignatureRef *result, std::string *errorMessage);
    agpu_error getOrCreateSpirVShaderInstance(const agpu::shader_signature_ref &signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, const std::string &entryPoint, GLShaderForSignatureRef *result, std::string *errorMessage);

    std::unordered_map<std::string, std::vector<TextureWithSamplerCombination>> textureWithSamplerCombinations;
};

} // End of namespace AgpuGL

#endif //_AGPU_GL_SHADER_HPP_
