#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

inline VkDescriptorType mapDescriptorType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case AGPU_SHADER_BINDING_TYPE_SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
    default: abort();
    }
}

_agpu_shader_signature_builder::_agpu_shader_signature_builder(agpu_device *device)
    : device(device)
{
}

void _agpu_shader_signature_builder::lostReferences()
{
    for(auto set : descriptorSets)
        vkDestroyDescriptorSetLayout(device->device, set, nullptr);
}

_agpu_shader_signature_builder *_agpu_shader_signature_builder::create(agpu_device *device)
{
    std::unique_ptr<_agpu_shader_signature_builder> result(new _agpu_shader_signature_builder(device));
    return result.release();
}

agpu_shader_signature* _agpu_shader_signature_builder::buildShaderSignature()
{
    VkPipelineLayoutCreateInfo layoutInfo;
    memset(&layoutInfo, 0, sizeof(layoutInfo));
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    if(!descriptorSets.empty())
    {
        layoutInfo.setLayoutCount = descriptorSets.size();
        layoutInfo.pSetLayouts = &descriptorSets[0];
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
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_signature_builder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_signature_builder::addBindingBank(agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    auto descriptorType = mapDescriptorType(type);
    for(uint i = 0; i < bindingPointCount; ++i)
    {
        VkDescriptorSetLayoutBinding binding;
        memset(&binding, 0, sizeof(binding));
        binding.binding = i;
        binding.descriptorType = descriptorType;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_ALL;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo setLayoutInfo;
    memset(&setLayoutInfo, 0, sizeof(setLayoutInfo));
    setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutInfo.bindingCount = bindings.size();
    printf("Descriptor set layout: %d\n", (int)bindings.size());
    setLayoutInfo.pBindings = &bindings[0];

    VkDescriptorSetLayout setLayout;
    auto error = vkCreateDescriptorSetLayout(device->device, &setLayoutInfo, nullptr, &setLayout);
    CONVERT_VULKAN_ERROR(error);

    descriptorSets.push_back(setLayout);

    elementDescription.push_back(ShaderSignatureElementDescription(true, type, descriptorType, bindingPointCount, maxBindings));

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

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBank(agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingBank(type, bindingPointCount, maxBindings);
}
