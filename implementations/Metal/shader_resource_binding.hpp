#ifndef AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_METAL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
public:
    _agpu_shader_resource_binding(agpu_device *device);
    void lostReferences();

    agpu_error bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer );
    agpu_error bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
    agpu_error bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp );
    agpu_error bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp );
    agpu_error createSampler ( agpu_int location, agpu_sampler_description* description );

    agpu_device *device;
};

#endif // AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
