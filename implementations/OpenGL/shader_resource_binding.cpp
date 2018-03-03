#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "texture.hpp"
#include "buffer.hpp"

inline GLenum mapMinFilter(agpu_filter filter)
{
    switch (filter)
    {
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:
        return GL_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:
    case AGPU_FILTER_ANISOTROPIC:
    default:
        return GL_LINEAR;
    }
}

inline GLenum mapMagFilter(agpu_filter filter)
{
    switch (filter)
    {
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:    return GL_NEAREST_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:     return GL_NEAREST_MIPMAP_LINEAR;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:     return GL_LINEAR_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:      return GL_LINEAR_MIPMAP_LINEAR;

    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:     return GL_NEAREST_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:      return GL_NEAREST_MIPMAP_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:      return GL_LINEAR_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:       return GL_LINEAR_MIPMAP_LINEAR;
    case AGPU_FILTER_ANISOTROPIC:                               return GL_LINEAR_MIPMAP_LINEAR;
    default:
        return GL_NEAREST;
    }
}

inline GLenum mapAddressMode(agpu_texture_address_mode mode)
{
    switch (mode)
    {
    default:
    case AGPU_TEXTURE_ADDRESS_MODE_WRAP:    return GL_REPEAT;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR:  return GL_MIRRORED_REPEAT;
    case AGPU_TEXTURE_ADDRESS_MODE_CLAMP:   return GL_CLAMP_TO_EDGE;
    case AGPU_TEXTURE_ADDRESS_MODE_BORDER:  return GL_CLAMP;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR_ONCE: return GL_MIRROR_CLAMP_TO_EDGE;
    }
}

_agpu_shader_resource_binding::_agpu_shader_resource_binding()
    : device(nullptr), signature(nullptr)
{
}

void _agpu_shader_resource_binding::lostReferences()
{
    for(auto &binding : uniformBuffers)
    {
        if (binding.buffer)
            binding.buffer->release();
    }

    for(auto &binding : storageBuffers)
    {
        if (binding.buffer)
            binding.buffer->release();
    }

    for(auto &binding : sampledImages)
    {
        if(binding.texture)
            binding.texture->release();
    }

    if(!samplers.empty())
    {
        device->onMainContextBlocking([&]{
            device->glDeleteSamplers(samplers.size(), &samplers[0]);
        });
    }

    signature->release();
}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_shader_signature *signature, int elementIndex)
{
	std::unique_ptr<agpu_shader_resource_binding> binding(new agpu_shader_resource_binding());
	binding->device = signature->device;
    binding->signature = signature;
    signature->retain();
	binding->elementIndex = elementIndex;

    const auto &bank = signature->elements[elementIndex];
    switch(bank.type)
    {
    case ShaderSignatureElementType::Element:
    case ShaderSignatureElementType::Bank:
        {
            binding->uniformBuffers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::UniformBuffer]);
            binding->storageBuffers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::StorageBuffer]);
            binding->sampledImages.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::SampledImage]);
            binding->samplers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::Sampler]);
        }
        break;
    default:
        abort();
        break;
    }

    return binding.release();
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
	if(location < 0)
		return AGPU_OK;

    const auto &bank = signature->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = uniformBuffers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::UniformBuffer]];

    std::unique_lock<std::mutex> l(bindMutex);
	if(uniform_buffer)
		uniform_buffer->retain();
	if(binding.buffer)
		binding.buffer->release();
 	binding.buffer = uniform_buffer;
	binding.range = offset != 0 || size != uniform_buffer->description.size;
	binding.offset = offset;
	binding.size = size;
	return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindStorageBuffer(agpu_int location, agpu_buffer* storage_buffer)
{
    CHECK_POINTER(storage_buffer);
    return bindStorageBufferRange(location, storage_buffer, 0, storage_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindStorageBufferRange(agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size)
{
	if(location < 0)
		return AGPU_OK;

    const auto &bank = signature->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = storageBuffers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::UniformBuffer]];

    std::unique_lock<std::mutex> l(bindMutex);
	if(storage_buffer)
		storage_buffer->retain();
	if(binding.buffer)
		binding.buffer->release();
 	binding.buffer = storage_buffer;
	binding.range = offset != 0 || size != storage_buffer->description.size;
	binding.offset = offset;
	binding.size = size;
	return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodClamp)
{
    CHECK_POINTER(texture);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE &&
       element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    texture->retain();
    auto &binding = sampledImages[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::SampledImage]];
    if(binding.texture)
        binding.texture->release();
    binding.texture = texture;

    // Store some of the texture parameters.
    binding.startMiplevel = startMiplevel;
    binding.miplevels = miplevels;
    binding.lodClamp = lodClamp;

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTextureArrayRange(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodClamp)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::createSampler(agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(description);
    if(location < 0)
        return AGPU_OK;
    const auto &bank = signature->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    auto sampler = samplers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::Sampler]];
    device->onMainContextBlocking([&]{
        std::unique_lock<std::mutex> l(bindMutex);
        device->glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mapMagFilter(description->filter));
        device->glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, mapMinFilter(description->filter));
        device->glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, mapAddressMode(description->address_u));
        device->glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, mapAddressMode(description->address_v));
        device->glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, mapAddressMode(description->address_w));
    });

    return AGPU_OK;
}

void _agpu_shader_resource_binding::activate()
{
    std::unique_lock<std::mutex> l(bindMutex);
    if(!uniformBuffers.empty())
        activateUniformBuffers();
    if(!storageBuffers.empty())
        activateStorageBuffers();
    if(!sampledImages.empty())
        activateSampledImages();
    if(!samplers.empty())
        activateSamplers();
}

void _agpu_shader_resource_binding::activateUniformBuffers()
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::UniformBuffer];
    activateBuffers(GL_UNIFORM_BUFFER, baseIndex, uniformBuffers);
}

void _agpu_shader_resource_binding::activateStorageBuffers()
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::StorageBuffer];
    activateBuffers(GL_SHADER_STORAGE_BUFFER, baseIndex, storageBuffers);
}

void _agpu_shader_resource_binding::activateBuffers(GLenum target, size_t baseIndex, std::vector<BufferBinding> &buffers)
{
    for (size_t i = 0; i < buffers.size(); ++i)
    {
        auto &binding = buffers[i];
        if (!binding.buffer)
            continue;

        if (binding.range)
            device->glBindBufferRange(target, GLuint(baseIndex + i), binding.buffer->handle, binding.offset, binding.size);
        else
            device->glBindBufferBase(target, GLuint(baseIndex + i), binding.buffer->handle);
    }
}

void _agpu_shader_resource_binding::activateSampledImages()
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::SampledImage];

    for (size_t i = 0; i < sampledImages.size(); ++i)
    {
        auto &binding = sampledImages[i];
        if(!binding.texture)
            continue;

        auto id = baseIndex + i;

        auto target = binding.texture->target;
        device->glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(target, binding.texture->handle);
    }
}

void _agpu_shader_resource_binding::activateSamplers()
{
    const auto &bank = signature->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::Sampler];
    for (size_t i = 0; i < samplers.size(); ++i)
    {
        auto sampler = samplers[i];
        if(sampler)
            device->glBindSampler(baseIndex + i, sampler);
    }
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
