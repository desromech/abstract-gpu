#ifndef AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
#define AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP

#include <vector>
#include "device.hpp"

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
public:
    _agpu_shader_resource_binding();

    void lostReferences();

    static agpu_shader_resource_binding *create(agpu_shader_signature *signature, agpu_uint element, UINT descriptorOffset);

    agpu_error bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer);
    agpu_error bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);

public:
    agpu_device *device;
    agpu_shader_signature *signature;
    agpu_shader_binding_type type;
    bool isBank;
    agpu_uint element;
    UINT descriptorOffset;
    std::vector<agpu_buffer*> buffers;
    std::vector<agpu_texture*> textures;

};

#endif //AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
