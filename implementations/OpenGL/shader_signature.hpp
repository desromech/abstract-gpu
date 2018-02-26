#ifndef AGPU_GL_SHADER_SIGNATURE_HPP
#define AGPU_GL_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

struct _agpu_shader_signature: public Object<_agpu_shader_signature>
{
public:
    _agpu_shader_signature();

    void lostReferences();

    static _agpu_shader_signature *create(agpu_device *device, agpu_shader_signature_builder *builder);

    agpu_shader_resource_binding* createShaderResourceBinding(agpu_uint element);

    int mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding);

    agpu_device *device;
    std::vector<ShaderSignatureElement> elements;
    agpu_uint uniformVariableCount;
};

#endif //AGPU_GL_SHADER_SIGNATURE_HPP
