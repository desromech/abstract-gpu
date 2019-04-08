#include <string.h>
#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

namespace AgpuGL
{

GLShaderSignatureBuilder::GLShaderSignatureBuilder()
{
    memset(bindingPointsUsed, 0, sizeof(bindingPointsUsed));
}

GLShaderSignatureBuilder::~GLShaderSignatureBuilder()
{
}

agpu::shader_signature_builder_ref GLShaderSignatureBuilder::create(const agpu::device_ref &device)
{
    auto result = agpu::makeObject<GLShaderSignatureBuilder> ();
    result.as<GLShaderSignatureBuilder> ()->device = device;
    return result;
}

agpu::shader_signature_ptr GLShaderSignatureBuilder::build()
{
    return GLShaderSignature::create(device, this).disown();
}

agpu_error GLShaderSignatureBuilder::addBindingConstant()
{
    bindingPointsUsed[(int)OpenGLResourceBindingType::UniformVariable] += 1;
    return AGPU_OK;
}

agpu_error GLShaderSignatureBuilder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    elements.push_back(ShaderSignatureElement(ShaderSignatureElementType::Element, maxBindings, bindingPointsUsed));
    auto &bank = elements.back();

    int bindingPointTypeIndex = (int)mapBindingType(type);
    bank.elements.push_back(ShaderSignatureBankElement(type, 1, bindingPointsUsed[bindingPointTypeIndex]));
    bank.elementTypeCounts[bindingPointTypeIndex] += 1;
    bindingPointsUsed[bindingPointTypeIndex] += 1;
    return AGPU_OK;
}

agpu_error GLShaderSignatureBuilder::beginBindingBank(agpu_uint maxBindings)
{
    elements.push_back(ShaderSignatureElement(ShaderSignatureElementType::Bank, maxBindings, bindingPointsUsed));
    return AGPU_OK;
}

agpu_error GLShaderSignatureBuilder::addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount)
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

} // End of namespace AgpuGL
