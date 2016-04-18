#include "shader_resource_binding.hpp"

_agpu_shader_resource_binding::_agpu_shader_resource_binding(agpu_device *device)
    : device(device)
{
}

void _agpu_shader_resource_binding::lostReferences()
{
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::createSampler ( agpu_int location, agpu_sampler_description* description )
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference ( agpu_shader_resource_binding* shader_resource_binding )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding ( agpu_shader_resource_binding* shader_resource_binding )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->release();
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBuffer(location, uniform_buffer);
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBufferRange(location, uniform_buffer, offset, size);
}

AGPU_EXPORT agpu_error agpuBindTexture ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindTexture(location, texture, startMiplevel, miplevels, lodclamp);
}

AGPU_EXPORT agpu_error agpuBindTextureArrayRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindTextureArrayRange(location, texture, startMiplevel, miplevels, firstElement, numberOfElements, lodclamp);
}

AGPU_EXPORT agpu_error agpuCreateSampler ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->createSampler(location, description);
}
