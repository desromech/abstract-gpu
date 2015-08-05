#ifndef AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
#define AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
public:
    _agpu_shader_resource_binding();

    void lostReferences();

    static agpu_shader_resource_binding *create(agpu_device *device, int bank);

    agpu_error bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer);
    agpu_error bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);

public:
    agpu_device *device;
    int bank;

    agpu_buffer *constantBuffers[4];

    size_t heapOffset;
};

#endif //AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
