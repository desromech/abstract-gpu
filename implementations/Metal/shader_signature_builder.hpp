#ifndef AGPU_METAL_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_METAL_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"
#include <vector>

enum class MetalResourceBindingType
{
    Buffer,
    Bytes,
    Texture,
    Sampler,
    Count,
};

inline MetalResourceBindingType mapBindingType(agpu_shader_binding_type type)
{
    switch(type)
    {
    case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE:
    case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE:
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_TEXEL_BUFFER:
    case AGPU_SHADER_BINDING_TYPE_STORAGE_TEXEL_BUFFER:
        return MetalResourceBindingType::Texture;

    case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER:
    case AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER:
        return MetalResourceBindingType::Buffer;

    case AGPU_SHADER_BINDING_TYPE_SAMPLER:
        return MetalResourceBindingType::Sampler;

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
    agpu_uint startIndices[(int)MetalResourceBindingType::Count];
    agpu_uint elementTypeCounts[(int)MetalResourceBindingType::Count];
    std::vector<ShaderSignatureBankElement> elements;
};

struct _agpu_shader_signature_builder: public Object<_agpu_shader_signature_builder>
{
public:
    _agpu_shader_signature_builder(agpu_device *device);
    void lostReferences();

    static _agpu_shader_signature_builder *create(agpu_device *device);

    agpu_shader_signature* build (  );
    agpu_error addBindingConstant (  );
    agpu_error addBindingElement ( agpu_shader_binding_type type, agpu_uint maxBindings );

    agpu_error beginBindingBank ( agpu_uint maxBindings );
    agpu_error addBindingBankElement ( agpu_shader_binding_type type, agpu_uint bindingPointCount );

    agpu_device *device;
    agpu_uint bindingPointsUsed[(int)MetalResourceBindingType::Count];
    std::vector<ShaderSignatureElement> elements;
};

#endif //AGPU_METAL_SHADER_SIGNATURE_BUILDER_HPP
