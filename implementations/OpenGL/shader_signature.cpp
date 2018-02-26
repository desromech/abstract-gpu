#include <string.h>
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

_agpu_shader_signature::_agpu_shader_signature()
    : device(nullptr)
{
}

void _agpu_shader_signature::lostReferences()
{
}

_agpu_shader_signature *_agpu_shader_signature::create(agpu_device *device, agpu_shader_signature_builder *builder)
{
    std::unique_ptr<agpu_shader_signature> result(new agpu_shader_signature());
    result->device = device;
    result->elements = builder->elements;
    result->uniformVariableCount = builder->bindingPointsUsed[(int)OpenGLResourceBindingType::UniformVariable];
    return result.release();
}

agpu_shader_resource_binding* _agpu_shader_signature::createShaderResourceBinding(agpu_uint element)
{
    if (element >= elements.size())
        return nullptr;

    return agpu_shader_resource_binding::create(this, element);
}

int _agpu_shader_signature::mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding)
{
    if(set >= elements.size())
        return -1;

    auto &bank = elements[set];
    if(binding >= bank.elements.size())
        return -1;

    auto &element = bank.elements[binding];
    if(element.type != type)
        return -1;

    return element.startIndex;
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
