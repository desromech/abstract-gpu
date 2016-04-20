#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

_agpu_shader_signature_builder::_agpu_shader_signature_builder(agpu_device *device)
    : device(device)
{
    memset(bindingPointsUsed, 0, sizeof(bindingPointsUsed));
}

void _agpu_shader_signature_builder::lostReferences()
{
}

_agpu_shader_signature_builder *_agpu_shader_signature_builder::create(agpu_device *device)
{
    return new agpu_shader_signature_builder(device);
}

agpu_shader_signature* _agpu_shader_signature_builder::build (  )
{
    return agpu_shader_signature::create(device, this);
}

agpu_error _agpu_shader_signature_builder::addBindingConstant (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_signature_builder::addBindingElement ( agpu_shader_binding_type type, agpu_uint maxBindings )
{
    elements.push_back(ShaderSignatureElement(type, 1, bindingPointsUsed[(int)type]));
    bindingPointsUsed[(int)type] += 1;
    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingBank ( agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings )
{
    elements.push_back(ShaderSignatureElement(type, bindingPointCount, bindingPointsUsed[(int)type]));
    bindingPointsUsed[(int)type] += bindingPointCount;
    return AGPU_OK;
}

// The exported C interface

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference ( agpu_shader_signature_builder* shader_signature_builder )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder ( agpu_shader_signature_builder* shader_signature_builder )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->release();
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature ( agpu_shader_signature_builder* shader_signature_builder )
{
    if(!shader_signature_builder)
        return nullptr;
    return shader_signature_builder->build();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant ( agpu_shader_signature_builder* shader_signature_builder )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingConstant();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingElement(type, maxBindings);
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBank ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings )
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingBank(type, bindingPointCount, maxBindings);
}
