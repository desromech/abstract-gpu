#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
#include "sampler.hpp"
#include "constants.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{

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
    AgpuProfileConstructor(AMtlShaderResourceBinding);
}

AMtlShaderResourceBinding::~AMtlShaderResourceBinding()
{
    AgpuProfileDestructor(AMtlShaderResourceBinding);
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
            binding->textureViews.resize(bank.elementTypeCounts[(int)MetalResourceBindingType::Texture]);
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

agpu_error AMtlShaderResourceBinding::bindSampledTextureView(agpu_int location, const agpu::texture_view_ref &view)
{
    CHECK_POINTER(view);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return AGPU_INVALID_OPERATION;

    textureViews[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Texture]] = view;
    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::bindStorageImageView(agpu_int location, const agpu::texture_view_ref &view)
{
    CHECK_POINTER(view);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    textureViews[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Texture]] = view;
    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::bindSampler(agpu_int location, const agpu::sampler_ref &sampler)
{
    CHECK_POINTER(sampler);
    if(location < 0)
        return AGPU_OK;
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    if(location >= bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;
        
    samplers[element.startIndex - bank.startIndices[(int)MetalResourceBindingType::Sampler]] = sampler;
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

    if(!textureViews.empty())
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
        auto &sampler = samplers[i];
        if(!sampler)
            continue;
            
        auto state = sampler.as<AMtlSampler> ()->handle;
        [encoder setVertexSamplerState: state atIndex: baseIndex + i];
        [encoder setFragmentSamplerState: state atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateTexturesOn(id<MTLRenderCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Texture];

    for(size_t i = 0; i < textureViews.size(); ++i)
    {
        auto &view = textureViews[i];
        if(!view)
            continue;
    
        view.as<AMtlTextureView> ()->activateOnRenderEncoder(encoder, baseIndex + i);
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

    if(!textureViews.empty())
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
        auto &sampler = samplers[i];
        if(!sampler)
            continue;
            
        auto state = sampler.as<AMtlSampler> ()->handle;
        [encoder setSamplerState: state atIndex: baseIndex + i];
    }

    return AGPU_OK;
}

agpu_error AMtlShaderResourceBinding::activateComputeTexturesOn(id<MTLComputeCommandEncoder> encoder)
{
    const auto &bank = signature.as<AMtlShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)MetalResourceBindingType::Texture];

    for(size_t i = 0; i < textureViews.size(); ++i)
    {
        auto &view = textureViews[i];
        if(!view)
            continue;
    
        view.as<AMtlTextureView> ()->activateOnComputeEncoder(encoder, baseIndex + i);
    }

    return AGPU_OK;
}

} // End of namespace AgpuMetal
