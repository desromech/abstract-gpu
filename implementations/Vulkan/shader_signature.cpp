#include "shader_signature.hpp"
#include "shader_signature_builder.hpp"
#include "shader_resource_binding.hpp"
#include <map>

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
    std::map<int, int> descriptorTypes;
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (auto &element : builder->elementDescription)
    {
        descriptorTypes.clear();
        for (auto& bindingDesc : element.bindings)
        {
            if (descriptorTypes.find(int(bindingDesc.descriptorType)) == descriptorTypes.end())
                descriptorTypes[int(bindingDesc.descriptorType)] = 0;
            descriptorTypes[int(bindingDesc.descriptorType)] += bindingDesc.descriptorCount;
        }
 
        poolSizes.clear();

        for(auto &pair : descriptorTypes)
        {
            VkDescriptorPoolSize poolSize;
            poolSize.descriptorCount = pair.second*element.maxBindings;
            poolSize.type = VkDescriptorType(pair.first);
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
