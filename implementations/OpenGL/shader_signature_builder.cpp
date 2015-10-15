#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

_agpu_shader_signature_builder::_agpu_shader_signature_builder()
{
    elementCount = 0;
    memset(elements, 0, sizeof(elements));
    memset(bindingCount, 0, sizeof(bindingCount));
}

void _agpu_shader_signature_builder::lostReferences()
{
}

_agpu_shader_signature_builder *_agpu_shader_signature_builder::create(agpu_device *device)
{
    std::unique_ptr<_agpu_shader_signature_builder> builder(new _agpu_shader_signature_builder());
    builder->device = device;
    return builder.release();
}

agpu_shader_signature* _agpu_shader_signature_builder::build()
{
    return agpu_shader_signature::create(device, elements);
}

agpu_error _agpu_shader_signature_builder::addBindingConstant()
{
    if (elementCount == 16)
        return AGPU_INVALID_OPERATION;

    auto &element = elements[elementCount++];
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_signature_builder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    if (elementCount == 16)
        return AGPU_INVALID_OPERATION;

    auto &element = elements[elementCount++];
    auto startIndex = bindingCount[type]++;
    element = ShaderSignatureElementDescription(false, type, 1, maxBindings, startIndex);
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingBank(agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings)
{
    if (elementCount == 16)
        return AGPU_INVALID_OPERATION;

    auto &element = elements[elementCount++];
    auto startIndex = bindingCount[type];
    bindingCount[type] += bindingPointCount;
    element = ShaderSignatureElementDescription(false, type, bindingPointCount, maxBindings, startIndex);
    return AGPU_OK;
}

// Exported C interface
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
    if (!shader_signature_builder)
        return nullptr;
    return shader_signature_builder->build();
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
