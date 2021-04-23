#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

namespace AgpuMetal
{
    
AMtlShaderSignatureBuilder::AMtlShaderSignatureBuilder(const agpu::device_ref &device)
    : device(device)
{
    memset(bindingPointsUsed, 0, sizeof(bindingPointsUsed));
}

AMtlShaderSignatureBuilder::~AMtlShaderSignatureBuilder()
{
}

agpu::shader_signature_builder_ref AMtlShaderSignatureBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AMtlShaderSignatureBuilder> (device);
}

agpu::shader_signature_ptr AMtlShaderSignatureBuilder::build()
{
    return AMtlShaderSignature::create(device, this).disown();
}

agpu_error AMtlShaderSignatureBuilder::addBindingConstant()
{
    bindingPointsUsed[(int)MetalResourceBindingType::Bytes] += 1;
    return AGPU_OK;
}

agpu_error AMtlShaderSignatureBuilder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    elements.push_back(ShaderSignatureElement(ShaderSignatureElementType::Element, maxBindings, bindingPointsUsed));
    auto &bank = elements.back();

    int bindingPointTypeIndex = (int)mapBindingType(type);
    bank.elements.push_back(ShaderSignatureBankElement(type, 1, bindingPointsUsed[bindingPointTypeIndex]));
    bank.elementTypeCounts[bindingPointTypeIndex] += 1;
    bindingPointsUsed[bindingPointTypeIndex] += 1;
    return AGPU_OK;
}

agpu_error AMtlShaderSignatureBuilder::beginBindingBank(agpu_uint maxBindings)
{
    elements.push_back(ShaderSignatureElement(ShaderSignatureElementType::Bank, maxBindings, bindingPointsUsed));
    return AGPU_OK;
}

agpu_error AMtlShaderSignatureBuilder::addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount)
{
    if(elements.empty() || elements.back().type != ShaderSignatureElementType::Bank)
        return AGPU_INVALID_OPERATION;

    auto &bank = elements.back();
    int bindingPointTypeIndex = (int)mapBindingType(type);
    for(agpu_uint i = 0; i < bindingPointCount; ++i)
    {
        bank.elements.push_back(ShaderSignatureBankElement(type, bindingPointCount, bindingPointsUsed[bindingPointTypeIndex]));
        bank.elementTypeCounts[bindingPointTypeIndex] += 1;
        bindingPointsUsed[bindingPointTypeIndex] += 1;
    }
    return AGPU_OK;
}

agpu_error AMtlShaderSignatureBuilder::addBindingBankArray(agpu_shader_binding_type type, agpu_uint size)
{
    return AGPU_UNIMPLEMENTED;
}

} // End of namespace AgpuMetal
