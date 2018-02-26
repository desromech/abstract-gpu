#include <string.h>
#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

_agpu_shader_signature_builder::_agpu_shader_signature_builder()
{
    memset(bindingPointsUsed, 0, sizeof(bindingPointsUsed));
}

void _agpu_shader_signature_builder::lostReferences()
{
}

_agpu_shader_signature_builder *_agpu_shader_signature_builder::create(agpu_device *device)
{
    std::unique_ptr<_agpu_shader_signature_builder> builder(new _agpu_shader_signature_builder());
    builder->device = device;
    return builder.release();
}

agpu_shader_signature* _agpu_shader_signature_builder::build()
{
    return agpu_shader_signature::create(device, this);
}

agpu_error _agpu_shader_signature_builder::addBindingConstant (  )
{
    bindingPointsUsed[(int)OpenGLResourceBindingType::UniformVariable] += 1;
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    elements.push_back(ShaderSignatureElement(ShaderSignatureElementType::Element, maxBindings, bindingPointsUsed));
    auto &bank = elements.back();

    int bindingPointTypeIndex = (int)mapBindingType(type);
    bank.elements.push_back(ShaderSignatureBankElement(type, 1, bindingPointsUsed[bindingPointTypeIndex]));
    bank.elementTypeCounts[bindingPointTypeIndex] += 1;
    bindingPointsUsed[bindingPointTypeIndex] += 1;
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::beginBindingBank ( agpu_uint maxBindings )
{
    elements.push_back(ShaderSignatureElement(ShaderSignatureElementType::Bank, maxBindings, bindingPointsUsed));
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingBankElement ( agpu_shader_binding_type type, agpu_uint bindingPointCount )
{
    if(elements.empty() || elements.back().type != ShaderSignatureElementType::Bank)
        return AGPU_INVALID_OPERATION;

    auto &bank = elements.back();
    int bindingPointTypeIndex = (int)mapBindingType(type);
    for(agpu_uint i = 0; i < bindingPointCount; ++i)
    {
        bank.elements.push_back(ShaderSignatureBankElement(type, 1, bindingPointsUsed[bindingPointTypeIndex]));
        bank.elementTypeCounts[bindingPointTypeIndex] += 1;
        bindingPointsUsed[bindingPointTypeIndex] += 1;
    }
    return AGPU_OK;
}

// The exported C interface

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference ( agpu_shader_signature_builder* shader_signature_builder )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder ( agpu_shader_signature_builder* shader_signature_builder )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->release();
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature ( agpu_shader_signature_builder* shader_signature_builder )
{
    if(!shader_signature_builder)
        return nullptr;
    return shader_signature_builder->build();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant ( agpu_shader_signature_builder* shader_signature_builder )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingConstant();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingElement(type, maxBindings);
}

AGPU_EXPORT agpu_error agpuBeginShaderSignatureBindingBank ( agpu_shader_signature_builder* shader_signature_builder, agpu_uint maxBindings )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->beginBindingBank(maxBindings);
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBankElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingBankElement(type, bindingPointCount);
}
