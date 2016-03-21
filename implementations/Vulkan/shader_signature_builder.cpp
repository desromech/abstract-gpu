#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

inline VkDescriptorType mapDescriptorType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SRV: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case AGPU_SHADER_BINDING_TYPE_UAV: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case AGPU_SHADER_BINDING_TYPE_CBV: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
    layoutInfo.setLayoutCount = descriptorSets.size();
    layoutInfo.pSetLayouts = &descriptorSets[0];

    VkPipelineLayout layout;
    auto error = vkCreatePipelineLayout(device->device, &layoutInfo, nullptr, &layout);
    if (error)
        return nullptr;

    std::unique_ptr<agpu_shader_signature> result(new agpu_shader_signature(device));
    result->layout = layout;
    result->builder = this;
    retain();
    return result.release();
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
    VkDescriptorSetLayoutBinding binding;
    memset(&binding, 0, sizeof(binding));
    binding.binding = 0;
    binding.descriptorType = mapDescriptorType(type);
    binding.descriptorCount = bindingPointCount;

    VkDescriptorSetLayoutCreateInfo setLayoutInfo;
    memset(&setLayoutInfo, 0, sizeof(setLayoutInfo));
    setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutInfo.bindingCount = 1;
    setLayoutInfo.pBindings = &binding;

    VkDescriptorSetLayout setLayout;
    auto error = vkCreateDescriptorSetLayout(device->device, &setLayoutInfo, nullptr, &setLayout);
    CONVERT_VULKAN_ERROR(error);

    descriptorSets.push_back(setLayout);
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
