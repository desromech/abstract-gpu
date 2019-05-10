#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

namespace AgpuMetal
{
    
AMtlShaderSignature::AMtlShaderSignature(const agpu::device_ref &device)
    : device(device)
{

}

AMtlShaderSignature::~AMtlShaderSignature()
{

}

agpu::shader_signature_ref AMtlShaderSignature::create(const agpu::device_ref &device, AMtlShaderSignatureBuilder *builder)
{
    auto result = agpu::makeObject<AMtlShaderSignature> (device);
    auto shaderSignature = result.as<AMtlShaderSignature> ();
    shaderSignature->elements = builder->elements;
    shaderSignature->pushConstantBufferSize = builder->bindingPointsUsed[(int)MetalResourceBindingType::Bytes]*4;
    shaderSignature->pushConstantBufferIndex = builder->bindingPointsUsed[(int)MetalResourceBindingType::Buffer];
    shaderSignature->buildMSLMapping();
    return result;
}

void AMtlShaderSignature::buildMSLMapping()
{
    for(size_t descriptorSet = 0; descriptorSet < elements.size(); ++descriptorSet)
    {
        auto &bank = elements[descriptorSet];
        for(size_t descriptorBinding = 0; descriptorBinding < bank.elements.size(); ++descriptorBinding)
        {
            auto &element = bank.elements[descriptorBinding];
            spirv_cross::MSLResourceBinding binding;
            binding.desc_set = descriptorSet;
            binding.binding = descriptorBinding;
            
            switch(mapBindingType(element.type))
            {
            case MetalResourceBindingType::Buffer:
                binding.msl_buffer = element.startIndex;
                resourceBindings.push_back(binding);
                break;
            case MetalResourceBindingType::Bytes:
                break;
            case MetalResourceBindingType::Texture:
                binding.msl_texture = element.startIndex;
                resourceBindings.push_back(binding);
                break;
            case MetalResourceBindingType::Sampler:
                binding.msl_sampler = element.startIndex;
                resourceBindings.push_back(binding);
                break;
            default:
                break;
            }
        }
    }
    
    // Map the push constants
    if(pushConstantBufferSize > 0)
    {
        spirv_cross::MSLResourceBinding binding;
        binding.desc_set = spirv_cross::kPushConstDescSet;
        binding.binding = spirv_cross::kPushConstBinding;
        
        binding.msl_buffer = pushConstantBufferIndex;
        resourceBindings.push_back(binding);
    }
}

agpu::shader_resource_binding_ptr AMtlShaderSignature::createShaderResourceBinding ( agpu_uint element )
{
    if(element >= elements.size())
        return nullptr;

    return AMtlShaderResourceBinding::create(device, refFromThis<agpu::shader_signature> (), element).disown();
}

int AMtlShaderSignature::mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding)
{
    if(set >= elements.size())
        return -1;

    auto &bank = elements[set];
    if(binding >= bank.elements.size())
        return -1;

    auto &element = bank.elements[binding];
    if(element.type != type)
        return -1;

    return element.startIndex;
}

} // End of namespace AgpuMetal
