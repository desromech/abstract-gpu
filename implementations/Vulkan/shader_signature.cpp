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
}

agpu::shader_resource_binding_ptr AVkShaderSignature::createShaderResourceBinding(agpu_uint element)
{
    if (element >= descriptorPools.size())
        return nullptr;

    auto &pool = descriptorPools[element];
    auto descriptorSet = pool->allocate();
    if(!descriptorSet)
        return nullptr;

    return AVkShaderResourceBinding::create(device,
            refFromThis<agpu::shader_signature> (),
            element, pool, descriptorSet)
            .disown();
}

agpu::shader_signature_ref AVkShaderSignature::create(const agpu::device_ref &device, AVkShaderSignatureBuilder *builder, VkPipelineLayout layout)
{
    // Allocate the signature and copy its parameters.
    auto result = agpu::makeObject<AVkShaderSignature> (device);
    auto signature = result.as<AVkShaderSignature> ();
    signature->layout = layout;

    signature->descriptorPools.reserve(builder->elementDescription.size());
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

        auto poolAllocator = std::make_shared<AVkDescriptorSetPool> ();
        poolAllocator->device = device;
        poolAllocator->setDescription = element;
        poolAllocator->elementSizes = poolSizes;
        signature->descriptorPools.push_back(poolAllocator);
    }

    builder->elementDescription.clear();
    return result;
}

} // End of namespace AgpuVulkan
