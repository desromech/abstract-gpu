#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "constants.hpp"

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

BufferBinding::~BufferBinding()
{
    reset();
}

void BufferBinding::reset()
{
    if(buffer)
        buffer->release();
    buffer = nullptr;
}

_agpu_shader_resource_binding::_agpu_shader_resource_binding(agpu_device *device)
    : device(device)
{
}

void _agpu_shader_resource_binding::lostReferences()
{
}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_device *device, agpu_shader_signature *signature, agpu_uint elementIndex)
{
    auto result = new agpu_shader_resource_binding(device);
    signature->retain();
    result->signature = signature;
    result->elementIndex = elementIndex;

    const auto &bank = signature->elements[elementIndex];
    switch(bank.type)
    {
    case ShaderSignatureElementType::Element:
    case ShaderSignatureElementType::Bank:
        {
            result->buffers.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Buffer]);
            result->textures.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Texture]);
            result->samplers.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Sampler]);
        }
        break;
    default:
        abort();
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

    const auto &bank = signature->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = buffers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Buffer]];
    uniform_buffer->retain();
    if(binding.buffer)
        binding.buffer->release();
    binding.buffer = uniform_buffer;
    binding.offset = offset;
    binding.size = size;
    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindStorageBuffer ( agpu_int location, agpu_buffer* storage_buffer )
{
    CHECK_POINTER(storage_buffer);
    return bindStorageBufferRange(location, storage_buffer, 0, storage_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindStorageBufferRange ( agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size )
{
    CHECK_POINTER(storage_buffer);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = buffers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Buffer]];
    storage_buffer->retain();
    if(binding.buffer)
        binding.buffer->release();
    binding.buffer = storage_buffer;
    binding.offset = offset;
    binding.size = size;
    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
{
    CHECK_POINTER(texture);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE &&
       element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    texture->retain();
    auto &binding = textures[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Texture]];
    if(binding)
        binding->release();
    binding = texture;
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
    const auto &bank = signature->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

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
    descriptor.compareFunction = mapCompareFunction(description->comparison_function);

    auto sampler = [device->device newSamplerStateWithDescriptor: descriptor];
    [descriptor release];
    if(!sampler)
        return AGPU_ERROR;

    auto &binding = samplers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Sampler]];
    if(binding)
        [binding release];
    binding = sampler;

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
{
    agpu_error error;
    if(!buffers.empty())
    {
        error = activateBuffersOn(vertexBufferCount, encoder);
        if(error != AGPU_OK)
            return error;
    }

    if(!textures.empty())
    {
        error = activateTexturesOn(encoder);
        if(error != AGPU_OK)
            return error;
    }

    if(!samplers.empty())
    {
        error = activateSamplersOn(encoder);
        if(error != AGPU_OK)
            return error;
    }

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::activateBuffersOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Buffer];
    for(size_t i = 0; i < buffers.size(); ++i)
    {
        auto &binding = buffers[i];

        // Ignore the buffers that are not binded.
        if(!binding.buffer)
            continue;

        [encoder setVertexBuffer: binding.buffer->handle offset: binding.offset atIndex: baseIndex + vertexBufferCount + i];
        [encoder setFragmentBuffer: binding.buffer->handle offset: binding.offset atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::activateSamplersOn(id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Sampler];

    for(size_t i = 0; i < samplers.size(); ++i)
    {
        auto state = samplers[i];
        if(!state)
            continue;

        [encoder setVertexSamplerState: state atIndex: baseIndex + i];
        [encoder setFragmentSamplerState: state atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::activateTexturesOn(id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Texture];

    for(size_t i = 0; i < textures.size(); ++i)
    {
        auto texture = textures[i];
        if(!texture)
            continue;

        [encoder setVertexTexture: texture->handle atIndex: baseIndex + i];
        [encoder setFragmentTexture: texture->handle atIndex: baseIndex + i];
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

AGPU_EXPORT agpu_error agpuBindStorageBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindStorageBuffer(location, storage_buffer);
}

AGPU_EXPORT agpu_error agpuBindStorageBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size )
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindStorageBufferRange(location, storage_buffer, offset, size);
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
