#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

_agpu_shader_signature::_agpu_shader_signature()
{
}

void _agpu_shader_signature::lostReferences()
{
}

_agpu_shader_signature *_agpu_shader_signature::create(agpu_device *device, ShaderSignatureElementDescription elements[16])
{
    std::unique_ptr<agpu_shader_signature> signature(new agpu_shader_signature());
    signature->device = device;
    memcpy(signature->elements, elements, sizeof(ShaderSignatureElementDescription) * 16);
    return signature.release();
}

agpu_shader_resource_binding* _agpu_shader_signature::createShaderResourceBinding(agpu_uint element)
{
    if (element >= 16)
        return nullptr;

    auto &el = elements[element];
    if (!el.valid)
        return nullptr;

    return agpu_shader_resource_binding::create(this, element);
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
