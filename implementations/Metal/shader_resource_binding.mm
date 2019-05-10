#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "constants.hpp"

namespace AgpuMetal
{
    
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
    buffer.reset();
}

AMtlShaderResourceBinding::AMtlShaderResourceBinding(const agpu::device_ref &device)
    : device(device)
{
}

AMtlShaderResourceBinding::~AMtlShaderResourceBinding()
{
}

agpu::shader_resource_binding_ref AMtlShaderResourceBinding::create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint elementIndex)
{
    auto result = agpu::makeObject<AMtlShaderResourceBinding> (device);
    auto binding = result.as<AMtlShaderResourceBinding> ();
    binding->signature = signature;
    binding->elementIndex = elementIndex;

    auto amtlSignature = signature.as<AMtlShaderSignature> ();
    const auto &bank = amtlSignature->elements[elementIndex];
    switch(bank.type)
    {
    case ShaderSignatureElementType::Element:
    case ShaderSignatureElementType::Bank:
        {
            binding->buffers.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Buffer]);
            binding->textures.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Texture]);
            binding->samplers.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Sampler]);
        }
        break;
    default:
        abort();
        break;
    }

    return result;
}

agpu_error AMtlShaderResourceBinding::bindUniformBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer.as<AMtlBuffer> ()->description.size);
}

agpu_error AMtlShaderResourceBinding::bindUniformBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(uniform_buffer);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = buffers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Buffer]];
    binding.buffer = uniform_buffer;
    binding.offset = offset;
    binding.size = size;
    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::bindStorageBuffer(agpu_int location, const agpu::buffer_ref &storage_buffer)
{
    CHECK_POINTER(storage_buffer);
    return bindStorageBufferRange(location, storage_buffer, 0, storage_buffer.as<AMtlBuffer> ()->description.size);
}

agpu_error AMtlShaderResourceBinding::bindStorageBufferRange ( agpu_int location, const agpu::buffer_ref &storage_buffer, agpu_size offset, agpu_size size )
{
    CHECK_POINTER(storage_buffer);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = buffers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Buffer]];
    binding.buffer = storage_buffer;
    binding.offset = offset;
    binding.size = size;
    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::bindTexture ( agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
{
    CHECK_POINTER(texture);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE &&
       element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    auto &binding = textures[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Texture]];
    binding = texture;
    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::bindTextureArrayRange ( agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
{
    CHECK_POINTER(texture);

    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlShaderResourceBinding::bindImage(agpu_int location, const agpu::texture_ref &texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format)
{
    CHECK_POINTER(texture);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    auto &binding = textures[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Texture]];
    binding = texture;
    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::createSampler ( agpu_int location, agpu_sampler_description* description )
{
    CHECK_POINTER(description);
    if(location < 0)
        return AGPU_OK;
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
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
    descriptor.maxAnisotropy = std::max(1.0f, description->maxanisotropy);
    descriptor.normalizedCoordinates = YES;
    descriptor.compareFunction = mapCompareFunction(description->comparison_function);

    auto sampler = [deviceForMetal->device newSamplerStateWithDescriptor: descriptor];
    [descriptor release];
    if(!sampler)
        return AGPU_ERROR;

    auto &binding = samplers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Sampler]];
    if(binding)
        [binding release];
    binding = sampler;

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
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

agpu_error AMtlShaderResourceBinding::activateBuffersOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Buffer];
    for(size_t i = 0; i < buffers.size(); ++i)
    {
        auto &binding = buffers[i];

        // Ignore the buffers that are not binded.
        if(!binding.buffer)
            continue;

        auto buffer = binding.buffer.as<AMtlBuffer> ();
        [encoder setVertexBuffer: buffer->handle offset: binding.offset atIndex: baseIndex + vertexBufferCount + i];
        [encoder setFragmentBuffer: buffer->handle offset: binding.offset atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateSamplersOn(id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
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

agpu_error AMtlShaderResourceBinding::activateTexturesOn(id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Texture];

    for(size_t i = 0; i < textures.size(); ++i)
    {
        auto texture = textures[i];
        if(!texture)
            continue;

        auto handle = texture.as<AMtlTexture> ()->handle;
        [encoder setVertexTexture: handle atIndex: baseIndex + i];
        [encoder setFragmentTexture: handle atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateComputeOn(id<MTLComputeCommandEncoder> encoder)
{
    agpu_error error;
    if(!buffers.empty())
    {
        error = activateComputeBuffersOn(encoder);
        if(error != AGPU_OK)
            return error;
    }

    if(!textures.empty())
    {
        error = activateComputeTexturesOn(encoder);
        if(error != AGPU_OK)
            return error;
    }

    if(!samplers.empty())
    {
        error = activateComputeSamplersOn(encoder);
        if(error != AGPU_OK)
            return error;
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateComputeBuffersOn(id<MTLComputeCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Buffer];
    for(size_t i = 0; i < buffers.size(); ++i)
    {
        auto &binding = buffers[i];

        // Ignore the buffers that are not binded.
        if(!binding.buffer)
            continue;

        auto handle = binding.buffer.as<AMtlBuffer> ()->handle;
        [encoder setBuffer: handle offset: binding.offset atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateComputeSamplersOn(id<MTLComputeCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Sampler];

    for(size_t i = 0; i < samplers.size(); ++i)
    {
        auto state = samplers[i];
        if(!state)
            continue;

        [encoder setSamplerState: state atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateComputeTexturesOn(id<MTLComputeCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Texture];

    for(size_t i = 0; i < textures.size(); ++i)
    {
        auto texture = textures[i];
        if(!texture)
            continue;

        auto handle = texture.as<AMtlTexture> ()->handle;
        [encoder setTexture: handle atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

} // End of namespace AgpuMetal
