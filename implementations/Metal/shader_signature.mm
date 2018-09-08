#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

_agpu_shader_signature::_agpu_shader_signature(agpu_device *device)
    : device(device)
{

}

void _agpu_shader_signature::lostReferences()
{

}

agpu_shader_signature *_agpu_shader_signature::create(agpu_device *device, agpu_shader_signature_builder *builder)
{
    auto result = new agpu_shader_signature(device);
    result->elements = builder->elements;
    result->pushConstantBufferSize = builder->bindingPointsUsed[(int)MetalResourceBindingType::Bytes]*4;
    result->pushConstantBufferIndex = builder->bindingPointsUsed[(int)MetalResourceBindingType::Buffer];
    result->buildMSLMapping();
    
    return result;
}

void _agpu_shader_signature::buildMSLMapping()
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

agpu_shader_resource_binding* _agpu_shader_signature::createShaderResourceBinding ( agpu_uint element )
{
    if(element >= elements.size())
        return nullptr;

    return agpu_shader_resource_binding::create(device, this, element);
}

int _agpu_shader_signature::mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding)
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

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderSignature ( agpu_shader_signature* shader_signature )
{
    CHECK_POINTER(shader_signature);
    return shader_signature->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignature ( agpu_shader_signature* shader_signature )
{
    CHECK_POINTER(shader_signature);
    return shader_signature->release();
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_shader_signature* shader_signature, agpu_uint element )
{
    if(!shader_signature)
        return nullptr;
    return shader_signature->createShaderResourceBinding(element);
}
