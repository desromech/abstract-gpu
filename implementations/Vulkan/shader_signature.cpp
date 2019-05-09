#include "shader_signature.hpp"
#include "shader_signature_builder.hpp"
#include "shader_resource_binding.hpp"

namespace AgpuVulkan
{

AVkShaderSignature::AVkShaderSignature(const agpu::device_ref &device)
    : device(device)
{
    layout = VK_NULL_HANDLE;
}

AVkShaderSignature::~AVkShaderSignature()
{
    if (layout)
        vkDestroyPipelineLayout(deviceForVk->device, layout, nullptr);
    for (auto pool : elementPools)
        vkDestroyDescriptorPool(deviceForVk->device, pool, nullptr);
    for(auto &element : elementDescription)
    {
        if(element.descriptorSetLayout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(deviceForVk->device, element.descriptorSetLayout, nullptr);
    }

}

agpu::shader_resource_binding_ptr AVkShaderSignature::createShaderResourceBinding(agpu_uint element)
{
    if (element >= elementPools.size())
        return nullptr;

    VkDescriptorSetAllocateInfo allocateInfo;
    memset(&allocateInfo, 0, sizeof(allocateInfo));
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = elementPools[element];
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &elementDescription[element].descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    auto error = vkAllocateDescriptorSets(deviceForVk->device, &allocateInfo, &descriptorSet);
    if (error)
    {
        printf("Failed to allocate descriptor set for %d\n", element);
        return nullptr;
    }

    return AVkShaderResourceBinding::create(device,
            refFromThis<agpu::shader_signature> (),
            element, descriptorSet,
            elementDescription[element])
            .disown();
}

agpu::shader_signature_ref AVkShaderSignature::create(const agpu::device_ref &device, AVkShaderSignatureBuilder *builder, VkPipelineLayout layout)
{
    // Allocate the signature and copy its parameters.
    auto result = agpu::makeObject<AVkShaderSignature> (device);
    auto signature = result.as<AVkShaderSignature> ();
    signature->layout = layout;

    signature->elementPools.reserve(signature->elementDescription.size());
    int descriptorTypes[VK_DESCRIPTOR_TYPE_RANGE_SIZE];
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (auto &element : builder->elementDescription)
    {
        memset(descriptorTypes, 0, sizeof(descriptorTypes));
        for(auto &bindingDesc : element.bindings)
            ++descriptorTypes[bindingDesc.descriptorType];

        poolSizes.clear();
        for(int i = 0; i < VK_DESCRIPTOR_TYPE_RANGE_SIZE; ++i)
        {
            if(!descriptorTypes[i])
                continue;

            VkDescriptorPoolSize poolSize;
            poolSize.descriptorCount = descriptorTypes[i]*element.maxBindings;
            poolSize.type = VkDescriptorType(VK_DESCRIPTOR_TYPE_BEGIN_RANGE + i);
            poolSizes.push_back(poolSize);
        }

        VkDescriptorPoolCreateInfo poolCreateInfo;
        memset(&poolCreateInfo, 0, sizeof(poolCreateInfo));
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.maxSets = element.maxBindings;
        poolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolCreateInfo.pPoolSizes = &poolSizes[0];
        poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        VkDescriptorPool pool;
        auto error = vkCreateDescriptorPool(deviceForVk->device, &poolCreateInfo, nullptr, &pool);
        if (error)
            return agpu::shader_signature_ref();

        signature->elementPools.push_back(pool);
    }

    signature->elementDescription.swap(builder->elementDescription);
    return result;
}

} // End of namespace AgpuVulkan
