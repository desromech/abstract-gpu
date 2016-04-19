#include "shader_resource_binding.hpp"
#include "buffer.hpp"

void UniformBufferBinding::reset()
{
    if(buffer)
        buffer->release();
}

_agpu_shader_resource_binding::_agpu_shader_resource_binding(agpu_device *device)
    : device(device)
{
}

void _agpu_shader_resource_binding::lostReferences()
{
}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_device *device, const ShaderSignatureElement &description)
{
    auto result = new agpu_shader_resource_binding(device);
    result->description = description;
    switch(description.type)
    {
    case AGPU_SHADER_BINDING_TYPE_SRV:
        result->textures.resize(description.bindingCount);
        break;
	case AGPU_SHADER_BINDING_TYPE_UAV:
        break;
	case AGPU_SHADER_BINDING_TYPE_CBV:
        result->uniformBindings.resize(description.bindingCount);
        break;
	case AGPU_SHADER_BINDING_TYPE_SAMPLER:
        break;
    default:
        break;
    }
    return result;
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer )
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
{
    CHECK_POINTER(uniform_buffer);
    if(location < 0)
        return AGPU_OK;
    if(location >= description.bindingCount)
        return AGPU_OUT_OF_BOUNDS;

    auto &binding = uniformBindings[location];
    uniform_buffer->retain();
    if(binding.buffer)
        binding.buffer->release();
    binding.buffer = uniform_buffer;
    return AGPU_OK;
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

agpu_error _agpu_shader_resource_binding::activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
{
    switch(description.type)
    {
	case AGPU_SHADER_BINDING_TYPE_CBV:
        return activateUniformBindingsOn(vertexBufferCount, encoder);
    default: return AGPU_UNSUPPORTED;
    }

}

agpu_error _agpu_shader_resource_binding::activateUniformBindingsOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
{
    for(size_t i = 0; i < uniformBindings.size(); ++i)
    {
        auto &binding = uniformBindings[i];
        if(!binding.buffer)
            return AGPU_INVALID_PARAMETER;

        [encoder setVertexBuffer: binding.buffer->handle offset: binding.offset atIndex: i + vertexBufferCount + description.startIndex];
        [encoder setFragmentBuffer: binding.buffer->handle offset: binding.offset atIndex: i + description.startIndex];
    }

    return AGPU_OK;
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
