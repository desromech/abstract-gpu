#ifndef _AGPU_GL_SHADER_HPP_
#define _AGPU_GL_SHADER_HPP_

#include "device.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>

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

namespace std
{
template<>
struct hash<TextureWithSamplerCombination>
{
    size_t operator()(const TextureWithSamplerCombination &combination) const
    {
        return combination.hash();
    }
};
}

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
};

struct _agpu_shader: public Object<_agpu_shader>
{
public:
    _agpu_shader();

    void lostReferences();

    static agpu_shader *createShader(agpu_device *device, agpu_shader_type type);
    agpu_error instanceForSignature(agpu_shader_signature *signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, agpu_shader_forSignature **result, std::string *errorMessage);

    agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
    agpu_error compileShader(agpu_cstring options);
    agpu_size getShaderCompilationLogLength();
    agpu_error getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer);

    std::vector<TextureWithSamplerCombination> &getTextureWithSamplerCombination();

public:
    //agpu_error compileGLSLSource(bool parseAgpuPragmas, agpu_string sourceText, agpu_string_length sourceTextLength);

    agpu_device *device;
    agpu_shader_forSignature *genericShaderInstance;
    agpu_shader_type type;
    bool compiled;

    agpu_shader_language rawSourceLanguage;
    std::vector<uint8_t> rawShaderSource;

private:
    void extractGenericShaderTextureWithSamplerCombinations();
    void extractSpirVTextureWithSamplerCombinations();

    agpu_error getOrCreateGenericShaderInstance(agpu_shader_signature *signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, agpu_shader_forSignature **result, std::string *errorMessage);
    agpu_error getOrCreateSpirVShaderInstance(agpu_shader_signature *signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, agpu_shader_forSignature **result, std::string *errorMessage);

    bool hasExtractedTextureWithSamplerCombinations;
    std::vector<TextureWithSamplerCombination> textureWithSamplerCombinations;
};


#endif //_AGPU_GL_SHADER_HPP_
