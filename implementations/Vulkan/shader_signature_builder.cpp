#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

namespace AgpuVulkan
{
inline agpu_uint mapMaxBindingsToBindingPoolBlockSize(agpu_uint maxBindings)
{
    if (maxBindings <= 64u)
        return maxBindings;
    if (maxBindings <= 256u)
        return 256u;
    if (maxBindings <= 4096u)
        return 4096u;

    return 1u<<16;
}

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

AVkShaderSignatureBuilder::AVkShaderSignatureBuilder(const agpu::device_ref &device)
    : device(device)
{
    currentElementSet = nullptr;
}

AVkShaderSignatureBuilder::~AVkShaderSignatureBuilder()
{
    for(auto &element : elementDescription)
    {
        if(element.descriptorSetLayout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(deviceForVk->device, element.descriptorSetLayout, nullptr);
    }
}

agpu::shader_signature_builder_ref AVkShaderSignatureBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AVkShaderSignatureBuilder> (device);
}

agpu::shader_signature_ptr AVkShaderSignatureBuilder::build()
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
        layoutInfo.setLayoutCount = (uint32_t)descriptorSets.size();
        layoutInfo.pSetLayouts = &descriptorSets[0];
    }

    if(!pushConstantRanges.empty())
    {
        layoutInfo.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();
        layoutInfo.pPushConstantRanges = &pushConstantRanges[0];
    }

    VkPipelineLayout layout;
    auto error = vkCreatePipelineLayout(deviceForVk->device, &layoutInfo, nullptr, &layout);
    if (error)
        return nullptr;

    auto result = AVkShaderSignature::create(device, this, layout);
    if (!result)
        vkDestroyPipelineLayout(deviceForVk->device, layout, nullptr);
    return result.disown();
}

agpu_error AVkShaderSignatureBuilder::addBindingConstant()
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

agpu_error AVkShaderSignatureBuilder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkShaderSignatureBuilder::finishBindingBank()
{
    if(currentElementSet)
    {
        VkDescriptorSetLayoutCreateInfo setLayoutInfo;
        memset(&setLayoutInfo, 0, sizeof(setLayoutInfo));
        setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = (uint32_t)currentElementSet->bindings.size();
        setLayoutInfo.pBindings = &currentElementSet->bindings[0];

        auto error = vkCreateDescriptorSetLayout(deviceForVk->device, &setLayoutInfo, nullptr, &currentElementSet->descriptorSetLayout);
        CONVERT_VULKAN_ERROR(error);
    }

    currentElementSet = nullptr;
    return AGPU_OK;
}

agpu_error AVkShaderSignatureBuilder::beginBindingBank ( agpu_uint maxBindings )
{
    auto error = finishBindingBank();
    if(error != AGPU_OK)
        return error;

    elementDescription.push_back(ShaderSignatureElementDescription(true, mapMaxBindingsToBindingPoolBlockSize(maxBindings)));
    currentElementSet = &elementDescription.back();
    return AGPU_OK;
}


agpu_error AVkShaderSignatureBuilder::addBindingBankArray(agpu_shader_binding_type type, agpu_uint size)
{
    if(!currentElementSet)
        return AGPU_INVALID_OPERATION;

    auto descriptorType = mapDescriptorType(type);
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = (uint32_t)currentElementSet->bindings.size();
    binding.descriptorType = descriptorType;
    binding.descriptorCount = size;
    binding.stageFlags = VK_SHADER_STAGE_ALL;
    currentElementSet->bindings.push_back(binding);
    currentElementSet->types.push_back(type);
    return AGPU_OK;
}

agpu_error AVkShaderSignatureBuilder::addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount)
{
    if(!currentElementSet)
        return AGPU_INVALID_OPERATION;

    auto descriptorType = mapDescriptorType(type);
    for(agpu_uint i = 0; i < bindingPointCount; ++i)
        addBindingBankArray(type, 1);

    return AGPU_OK;
}

} // End of namespace AgpuVulkan
