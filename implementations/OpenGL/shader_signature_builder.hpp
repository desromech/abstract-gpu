#ifndef AGPU_GL_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_GL_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"
#include <string.h>
#include <vector>

enum class OpenGLResourceBindingType
{
    SampledImage,
    StorageImage,
    UniformTexelBuffer,
    UniformStorageTexelBuffer,

    UniformBuffer,
    StorageBuffer,

    Sampler,
    UniformVariable,
    Count,
};

inline OpenGLResourceBindingType mapBindingType(agpu_shader_binding_type type)
{
    switch(type)
    {
    case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE:
        return OpenGLResourceBindingType::SampledImage;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE:
        return OpenGLResourceBindingType::StorageImage;
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_TEXEL_BUFFER:
        return OpenGLResourceBindingType::UniformTexelBuffer;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_TEXEL_BUFFER:
        return OpenGLResourceBindingType::UniformStorageTexelBuffer;

    case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER:
        return OpenGLResourceBindingType::UniformBuffer;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER:
        return OpenGLResourceBindingType::StorageBuffer;

    case AGPU_SHADER_BINDING_TYPE_SAMPLER:
        return OpenGLResourceBindingType::Sampler;
    default:
        abort();
        break;
    }
}

struct ShaderSignatureBankElement
{
    ShaderSignatureBankElement() {}
    ShaderSignatureBankElement(agpu_shader_binding_type type, agpu_uint bindingCount, agpu_uint startIndex)
        : type(type), bindingCount(bindingCount), startIndex(startIndex)
    {
    }

    agpu_shader_binding_type type;
    agpu_uint bindingCount;
    agpu_uint startIndex;
};

enum class ShaderSignatureElementType
{
    Constant,
    Element,
    Bank,
};

struct ShaderSignatureElement
{
    ShaderSignatureElement(ShaderSignatureElementType type, agpu_uint maxBindings, agpu_uint *newStartIndices)
        : type(type), maxBindings(maxBindings)
    {
        memset(elementTypeCounts, 0, sizeof(elementTypeCounts));
        memcpy(startIndices, newStartIndices, sizeof(startIndices));
    }

    ShaderSignatureElementType type;
    agpu_uint maxBindings;
    agpu_uint startIndices[(int)OpenGLResourceBindingType::Count];
    agpu_uint elementTypeCounts[(int)OpenGLResourceBindingType::Count];
    std::vector<ShaderSignatureBankElement> elements;
};

struct _agpu_shader_signature_builder: public Object<_agpu_shader_signature_builder>
{
public:
    _agpu_shader_signature_builder();

    void lostReferences();

    static _agpu_shader_signature_builder *create(agpu_device *device);

    agpu_shader_signature* build();
    agpu_error addBindingConstant (  );
    agpu_error addBindingElement ( agpu_shader_binding_type type, agpu_uint maxBindings );

    agpu_error beginBindingBank ( agpu_uint maxBindings );
    agpu_error addBindingBankElement ( agpu_shader_binding_type type, agpu_uint bindingPointCount );

    agpu_device *device;
    agpu_uint bindingPointsUsed[(int)OpenGLResourceBindingType::Count];
    std::vector<ShaderSignatureElement> elements;
};

#endif //AGPU_GL_SHADER_SIGNATURE_BUILDER_HPP
