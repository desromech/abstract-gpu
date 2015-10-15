#ifndef AGPU_GL_SHADER_SIGNATURE_HPP
#define AGPU_GL_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

struct _agpu_shader_signature: public Object<_agpu_shader_signature>
{
public:
    _agpu_shader_signature();

    void lostReferences();

    static _agpu_shader_signature *create(agpu_device *device, ShaderSignatureElementDescription elements[16]);

    agpu_shader_resource_binding* createShaderResourceBinding(agpu_uint element);

public:

    agpu_device *device;
    ShaderSignatureElementDescription elements[16];
};

#endif //AGPU_GL_SHADER_SIGNATURE_HPP
