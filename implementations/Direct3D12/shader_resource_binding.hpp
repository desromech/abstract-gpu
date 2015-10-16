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

    agpu_error bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodClamp);
    agpu_error bindTextureArrayRange (agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodClamp);
    agpu_error createSampler(agpu_int location, agpu_sampler_description* description);

public:
    agpu_device *device;
    agpu_shader_signature *signature;
    agpu_shader_binding_type type;
    bool isBank;
    agpu_uint element;
    UINT descriptorOffset;
    std::vector<agpu_buffer*> buffers;
    std::vector<agpu_texture*> textures;
    agpu_int samplerCount;

private:
    std::mutex bindMutex;

};

#endif //AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
