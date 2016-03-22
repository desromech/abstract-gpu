#include "shader_signature.hpp"
#include "shader_signature_builder.hpp"
#include "shader_resource_binding.hpp"

_agpu_shader_signature::_agpu_shader_signature(agpu_device *device)
    : device(device)
{
    layout = nullptr;
    builder = nullptr;
}

void _agpu_shader_signature::lostReferences()
{
    if (layout)
        vkDestroyPipelineLayout(device->device, layout, nullptr);
    for (auto pool : elementPools)
        vkDestroyDescriptorPool(device->device, pool, nullptr);

    builder->release();
}

agpu_shader_resource_binding* _agpu_shader_signature::createShaderResourceBinding(agpu_uint element)
{
    if (element >= elementPools.size())
        return nullptr;

    VkDescriptorSetAllocateInfo allocateInfo;
    memset(&allocateInfo, 0, sizeof(allocateInfo));
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = elementPools[element];
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &builder->descriptorSets[element];

    VkDescriptorSet descriptorSet;
    auto error = vkAllocateDescriptorSets(device->device, &allocateInfo, &descriptorSet);
    if (error)
        return nullptr;

    return agpu_shader_resource_binding::create(device, this, element, descriptorSet, elementDescription[element]);
}

agpu_shader_signature *_agpu_shader_signature::create(agpu_device *device, agpu_shader_signature_builder *builder, VkPipelineLayout layout)
{
    // Allocate the signature and copy its parameters.
    auto result = new agpu_shader_signature(device);
    result->layout = layout;
    result->builder = builder;
    builder->retain();

    result->elementDescription = builder->elementDescription;
    result->elementPools.reserve(result->elementDescription.size());
    for (auto &element : result->elementDescription)
    {
        VkDescriptorPoolSize poolSize;
        poolSize.descriptorCount = element.bindingPointCount;
        poolSize.type = element.vulkanDescriptionType;

        VkDescriptorPoolCreateInfo poolCreateInfo;
        memset(&poolCreateInfo, 0, sizeof(poolCreateInfo));
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.maxSets = element.maxBindings;
        poolCreateInfo.poolSizeCount = 1;
        poolCreateInfo.pPoolSizes = &poolSize;
        poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        VkDescriptorPool pool;
        auto error = vkCreateDescriptorPool(device->device, &poolCreateInfo, nullptr, &pool);
        if (error)
        {
            result->release();
            return nullptr;
        }

        result->elementPools.push_back(pool);
    }

    return result;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderSignature(agpu_shader_signature* shader_signature)
{
    CHECK_POINTER(shader_signature);
    return shader_signature->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignature(agpu_shader_signature* shader_signature)
{
    CHECK_POINTER(shader_signature);
    return shader_signature->release();
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding(agpu_shader_signature* shader_signature, agpu_uint element)
{
    if (!shader_signature)
        return nullptr;

    return shader_signature->createShaderResourceBinding(element);
}
