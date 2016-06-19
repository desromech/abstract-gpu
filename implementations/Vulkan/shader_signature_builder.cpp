#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

inline VkDescriptorType mapDescriptorType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case AGPU_SHADER_BINDING_TYPE_SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
    default: abort();
    }
}

_agpu_shader_signature_builder::_agpu_shader_signature_builder(agpu_device *device)
    : device(device)
{
    currentElementSet = nullptr;
}

void _agpu_shader_signature_builder::lostReferences()
{
    for(auto &element : elementDescription)
    {
        if(element.descriptorSetLayout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(device->device, element.descriptorSetLayout, nullptr);
    }
}

_agpu_shader_signature_builder *_agpu_shader_signature_builder::create(agpu_device *device)
{
    std::unique_ptr<_agpu_shader_signature_builder> result(new _agpu_shader_signature_builder(device));
    return result.release();
}

agpu_shader_signature* _agpu_shader_signature_builder::buildShaderSignature()
{
    // Finish the last binding bank.
    finishBindingBank();

    // Put the descriptor sets in a vector.
    std::vector<VkDescriptorSetLayout> descriptorSets;
    descriptorSets.reserve(elementDescription.size());
    for(auto &element : elementDescription)
        descriptorSets.push_back(element.descriptorSetLayout);

    // Create the pipeline layout.
    VkPipelineLayoutCreateInfo layoutInfo;
    memset(&layoutInfo, 0, sizeof(layoutInfo));
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    if(!descriptorSets.empty())
    {
        layoutInfo.setLayoutCount = descriptorSets.size();
        layoutInfo.pSetLayouts = &descriptorSets[0];
    }

    if(!pushConstantRanges.empty())
    {
        layoutInfo.pushConstantRangeCount = pushConstantRanges.size();
        layoutInfo.pPushConstantRanges = &pushConstantRanges[0];
    }

    VkPipelineLayout layout;
    auto error = vkCreatePipelineLayout(device->device, &layoutInfo, nullptr, &layout);
    if (error)
        return nullptr;

    auto result = agpu_shader_signature::create(device, this, layout);
    if (!result)
        vkDestroyPipelineLayout(device->device, layout, nullptr);
    return result;
}

agpu_error _agpu_shader_signature_builder::addBindingConstant()
{
    if(pushConstantRanges.empty())
    {
        VkPushConstantRange range;
        range.offset = 0;
        range.size = 0;
        range.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRanges.push_back(range);
    }

    pushConstantRanges.back().size += 4;
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_signature_builder::finishBindingBank()
{
    if(currentElementSet)
    {
        VkDescriptorSetLayoutCreateInfo setLayoutInfo;
        memset(&setLayoutInfo, 0, sizeof(setLayoutInfo));
        setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = currentElementSet->bindings.size();
        setLayoutInfo.pBindings = &currentElementSet->bindings[0];

        auto error = vkCreateDescriptorSetLayout(device->device, &setLayoutInfo, nullptr, &currentElementSet->descriptorSetLayout);
        CONVERT_VULKAN_ERROR(error);
    }

    currentElementSet = nullptr;
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::beginBindingBank ( agpu_uint maxBindings )
{
    auto error = finishBindingBank();
    if(error != AGPU_OK)
        return error;

    elementDescription.push_back(ShaderSignatureElementDescription(true, maxBindings));
    currentElementSet = &elementDescription.back();
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount)
{
    if(!currentElementSet)
        return AGPU_INVALID_OPERATION;

    auto descriptorType = mapDescriptorType(type);
    for(uint i = 0; i < bindingPointCount; ++i)
    {
        VkDescriptorSetLayoutBinding binding;
        memset(&binding, 0, sizeof(binding));
        binding.binding = currentElementSet->bindings.size();
        binding.descriptorType = descriptorType;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_ALL;
        currentElementSet->bindings.push_back(binding);
        currentElementSet->types.push_back(type);
    }

    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference(agpu_shader_signature_builder* shader_signature_builder)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder(agpu_shader_signature_builder* shader_signature_builder)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->release();
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature(agpu_shader_signature_builder* shader_signature_builder)
{
    if(!shader_signature_builder)
        return nullptr;
    return shader_signature_builder->buildShaderSignature();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant(agpu_shader_signature_builder* shader_signature_builder)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingConstant();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement(agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings)
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
