#include "shader_signature.hpp"
#include "shader_signature_builder.hpp"

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
    builder->release();
}

agpu_shader_resource_binding* _agpu_shader_signature::createShaderResourceBinding(agpu_uint element)
{
    return nullptr;
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
