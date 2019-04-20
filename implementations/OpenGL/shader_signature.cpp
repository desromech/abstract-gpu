#include <string.h>
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

namespace AgpuGL
{

GLShaderSignature::GLShaderSignature()
    : device(nullptr)
{
}

GLShaderSignature::~GLShaderSignature()
{
}

agpu::shader_signature_ref GLShaderSignature::create(const agpu::device_ref &device, const GLShaderSignatureBuilder *builder)
{
    auto result = agpu::makeObject<GLShaderSignature> ();
    auto signature = result.as<GLShaderSignature> ();
    signature->device = device;
    signature->elements = builder->elements;
    signature->uniformVariableCount = builder->bindingPointsUsed[(int)OpenGLResourceBindingType::UniformVariable];
    memcpy(signature->bindingPointsUsed, builder->bindingPointsUsed, sizeof(signature->bindingPointsUsed));
    return result;
}

agpu::shader_resource_binding_ptr GLShaderSignature::createShaderResourceBinding(agpu_uint element)
{
    if (element >= elements.size())
        return nullptr;

    return GLShaderResourceBinding::create(refFromThis<agpu::shader_signature> (), element).disown();
}

int GLShaderSignature::mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding)
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

} // End of namespace AgpuGL
