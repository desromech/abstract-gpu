#ifndef AGPU_VULKAN_SHADER_SIGNATURE_HPP
#define AGPU_VULKAN_SHADER_SIGNATURE_HPP

#include "device.hpp"

struct _agpu_shader_signature : public Object<_agpu_shader_signature>
{
    _agpu_shader_signature(agpu_device *device);
    void lostReferences();

    agpu_device *device;

    agpu_shader_resource_binding* createShaderResourceBinding(agpu_uint element);
};

#endif //AGPU_VULKAN_SHADER_SIGNATURE_HPP
