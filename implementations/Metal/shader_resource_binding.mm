#include "shader_resource_binding.hpp"
#include "buffer.hpp"
#include "texture.hpp"

inline MTLSamplerMinMagFilter mapMinFilter(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR: return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST: return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:  return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST: return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:  return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:  return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:   return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_ANISOTROPIC:                           return MTLSamplerMinMagFilterLinear;
    }
}

inline MTLSamplerMinMagFilter mapMagFilter(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST: return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:  return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:  return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:   return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:  return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:   return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:   return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:    return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_ANISOTROPIC:                            return MTLSamplerMinMagFilterLinear;
    }
}

inline MTLSamplerMipFilter mapMipmapMode(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST: return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:  return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:  return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:   return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:  return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:   return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:   return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:    return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_ANISOTROPIC:                            return MTLSamplerMipFilterLinear;
    }
}

inline MTLSamplerAddressMode mapAddressMode(agpu_texture_address_mode mode)
{
    switch (mode)
    {
    default:
    case AGPU_TEXTURE_ADDRESS_MODE_WRAP:    return MTLSamplerAddressModeRepeat;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR:  return MTLSamplerAddressModeMirrorRepeat;
    case AGPU_TEXTURE_ADDRESS_MODE_CLAMP:   return MTLSamplerAddressModeClampToEdge;
    case AGPU_TEXTURE_ADDRESS_MODE_BORDER:  return MTLSamplerAddressModeClampToZero;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR_ONCE: return MTLSamplerAddressModeMirrorClampToEdge;
    }
}

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
        result->samplerStates.resize(description.bindingCount);
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
    CHECK_POINTER(texture);
    if(location < 0)
        return AGPU_OK;
    if(location >= textures.size())
        return AGPU_OUT_OF_BOUNDS;

    texture->retain();
    if(textures[location])
        textures[location]->release();
    textures[location] = texture;
    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
{
    CHECK_POINTER(texture);

    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::createSampler ( agpu_int location, agpu_sampler_description* description )
{
    CHECK_POINTER(description);
    if(location < 0)
        return AGPU_OK;
    if(location >= samplerStates.size())
        return AGPU_OUT_OF_BOUNDS;

    auto descriptor = [MTLSamplerDescriptor new];
    descriptor.sAddressMode = mapAddressMode(description->address_u);
    descriptor.tAddressMode = mapAddressMode(description->address_v);
    descriptor.rAddressMode = mapAddressMode(description->address_w);
    descriptor.minFilter = mapMinFilter(description->filter);
    descriptor.magFilter = mapMinFilter(description->filter);
    descriptor.mipFilter = mapMipmapMode(description->filter);
    descriptor.lodMinClamp = description->min_lod;
    descriptor.lodMaxClamp = description->max_lod;
    descriptor.maxAnisotropy = description->maxanisotropy;
    descriptor.normalizedCoordinates = YES;

    auto sampler = [device->device newSamplerStateWithDescriptor: descriptor];
    [descriptor release];
    if(!sampler)
        return AGPU_ERROR;

    if(this->samplerStates[location])
        [this->samplerStates[location] release];
    this->samplerStates[location] = sampler;

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
{
    switch(description.type)
    {
	case AGPU_SHADER_BINDING_TYPE_CBV:
        return activateUniformBindingsOn(vertexBufferCount, encoder);
    case AGPU_SHADER_BINDING_TYPE_SRV:
        return activateTexturesOn(encoder);
    case AGPU_SHADER_BINDING_TYPE_SAMPLER:
        return activateSamplersOn(encoder);
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

agpu_error _agpu_shader_resource_binding::activateSamplersOn(id<MTLRenderCommandEncoder> encoder)
{
    for(size_t i = 0; i < samplerStates.size(); ++i)
    {
        auto state = samplerStates[i];
        if(!state)
            return AGPU_INVALID_PARAMETER;

        [encoder setVertexSamplerState: state atIndex: i + description.startIndex ];
        [encoder setFragmentSamplerState: state atIndex: i + description.startIndex ];
    }

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::activateTexturesOn(id<MTLRenderCommandEncoder> encoder)
{
    for(size_t i = 0; i < textures.size(); ++i)
    {
        auto texture = textures[i];
        if(!texture)
            return AGPU_INVALID_PARAMETER;

        [encoder setVertexTexture: texture->handle atIndex: i + description.startIndex ];
        [encoder setFragmentTexture: texture->handle atIndex: i + description.startIndex ];
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
