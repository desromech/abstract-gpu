#ifndef AGPU_METAL_SHADER_SIGNATURE_HPP
#define AGPU_METAL_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

struct _agpu_shader_signature : public Object<_agpu_shader_signature>
{
public:
    _agpu_shader_signature(agpu_device *device);
    void lostReferences();

    static agpu_shader_signature *create(agpu_device *device, agpu_shader_signature_builder *builder);

    agpu_device *device;
    std::vector<ShaderSignatureElement> elements;
};

#endif // AGPU_METAL_SHADER_SIGNATURE_HPP
