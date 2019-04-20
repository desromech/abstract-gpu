#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "constants.hpp"

namespace AgpuGL
{

inline GLenum mapMagFilter(agpu_filter filter)
{
    switch (filter)
    {
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:
        return GL_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:
        return GL_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:
        return GL_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:
    case AGPU_FILTER_ANISOTROPIC:
    default:
        return GL_LINEAR;
    }
}

inline GLenum mapMinFilter(agpu_filter filter)
{
    switch (filter)
    {
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:    return GL_NEAREST_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:     return GL_NEAREST_MIPMAP_LINEAR;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:     return GL_NEAREST_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:      return GL_NEAREST_MIPMAP_LINEAR;

    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:     return GL_LINEAR_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:      return GL_LINEAR_MIPMAP_LINEAR;
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

GLShaderResourceBinding::GLShaderResourceBinding()
{
}

GLShaderResourceBinding::~GLShaderResourceBinding()
{
    if(!samplers.empty())
    {
        deviceForGL->onMainContextBlocking([&]{
            deviceForGL->glDeleteSamplers((GLsizei)samplers.size(), &samplers[0]);
        });
    }
}

agpu::shader_resource_binding_ref GLShaderResourceBinding::create(const agpu::shader_signature_ref &signature, int elementIndex)
{
    auto result = agpu::makeObject<GLShaderResourceBinding> ();
	auto binding = result.as<GLShaderResourceBinding> ();
    auto glSignature = signature.as<GLShaderSignature>();
	binding->device = glSignature->device;
    binding->signature = signature;
	binding->elementIndex = elementIndex;

    const auto &bank = glSignature->elements[elementIndex];
    switch(bank.type)
    {
    case ShaderSignatureElementType::Element:
    case ShaderSignatureElementType::Bank:
        {
            binding->uniformBuffers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::UniformBuffer]);
            binding->storageBuffers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::StorageBuffer]);
            binding->sampledImages.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::SampledImage]);
            binding->samplers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::Sampler]);
            if(!binding->samplers.empty())
            {
                const auto &device = binding->device;
                deviceForGL->onMainContextBlocking([&]{
                    deviceForGL->glGenSamplers((GLsizei)binding->samplers.size(), &binding->samplers[0]);
                });
            }
        }
        break;
    default:
        abort();
        break;
    }

    return result;
}

agpu_error GLShaderResourceBinding::bindUniformBuffer(agpu_int location, const agpu::buffer_ref& uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer.as<GLBuffer>()->description.size);
}

agpu_error GLShaderResourceBinding::bindUniformBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size)
{
	if(location < 0)
		return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = uniformBuffers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::UniformBuffer]];

 	binding.buffer = uniform_buffer;
	binding.range = offset != 0 || size != uniform_buffer.as<GLBuffer>()->description.size;
	binding.offset = offset;
	binding.size = size;
	return AGPU_OK;
}

agpu_error GLShaderResourceBinding::bindStorageBuffer(agpu_int location, const agpu::buffer_ref &storage_buffer)
{
    CHECK_POINTER(storage_buffer);
    return bindStorageBufferRange(location, storage_buffer, 0, storage_buffer.as<GLBuffer>()->description.size);
}

agpu_error GLShaderResourceBinding::bindStorageBufferRange(agpu_int location, const agpu::buffer_ref &storage_buffer, agpu_size offset, agpu_size size)
{
	if(location < 0)
		return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    auto &binding = storageBuffers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::StorageBuffer]];
 	binding.buffer = storage_buffer;
	binding.range = offset != 0 || size != storage_buffer.as<GLBuffer>()->description.size;
	binding.offset = offset;
	binding.size = size;
	return AGPU_OK;
}

agpu_error GLShaderResourceBinding::bindTexture(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodClamp)
{
    CHECK_POINTER(texture);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE &&
       element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    auto &binding = sampledImages[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::SampledImage]];
    binding.texture = texture;

    // Store some of the texture parameters.
    binding.startMiplevel = startMiplevel;
    binding.miplevels = miplevels;
    binding.lodClamp = lodClamp;

    return AGPU_OK;
}

TextureBinding *GLShaderResourceBinding::getTextureBindingAt(agpu_int location)
{
    if(location < 0)
        return nullptr;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return nullptr;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE &&
       element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return nullptr;

    return &sampledImages[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::SampledImage]];
}

agpu_error GLShaderResourceBinding::bindTextureArrayRange(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodClamp)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error GLShaderResourceBinding::bindImage(agpu_int location, const agpu::texture_ref &texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error GLShaderResourceBinding::createSampler(agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(description);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    auto sampler = samplers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::Sampler]];
    auto glDevice = device.as<GLDevice> ();
    glDevice->onMainContextBlocking([&]{
        glDevice->glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mapMagFilter(description->filter));
        glDevice->glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, mapMinFilter(description->filter));
        glDevice->glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, mapAddressMode(description->address_u));
        glDevice->glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, mapAddressMode(description->address_v));
        glDevice->glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, description->min_lod);
        glDevice->glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, description->max_lod);
        glDevice->glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, description->comparison_enabled ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
        glDevice->glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, mapCompareFunction(description->comparison_function));
    });

    return AGPU_OK;
}


GLuint GLShaderResourceBinding::getSamplerAt(agpu_int location)
{
    if(location < 0)
        return 0;
    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return 0;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return 0;

    return samplers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::Sampler]];
}

void GLShaderResourceBinding::activate()
{
    if(!uniformBuffers.empty())
        activateUniformBuffers();
    if(!storageBuffers.empty())
        activateStorageBuffers();
}

void GLShaderResourceBinding::activateUniformBuffers()
{
    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::UniformBuffer];
    activateBuffers(GL_UNIFORM_BUFFER, baseIndex, uniformBuffers);
}

void GLShaderResourceBinding::activateStorageBuffers()
{
    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::StorageBuffer];
    activateBuffers(GL_SHADER_STORAGE_BUFFER, baseIndex, storageBuffers);
}

void GLShaderResourceBinding::activateBuffers(GLenum target, size_t baseIndex, std::vector<BufferBinding> &buffers)
{
    auto glDevice = device.as<GLDevice> ();
    for (size_t i = 0; i < buffers.size(); ++i)
    {
        auto &binding = buffers[i];
        if (!binding.buffer)
            continue;

        //printf("Bind buffer %d handle %d target %d offset %d size %d[range: %d]\n", int(baseIndex + i), binding.buffer->handle, target, int(binding.offset), int(binding.size), binding.range);
        //if(baseIndex + i == 1)
        //    binding.buffer->dumpToFile("camera.bin");
        if (binding.range)
            glDevice->glBindBufferRange(target, GLuint(baseIndex + i), binding.buffer.as<GLBuffer>()->handle, binding.offset, binding.size);
        else
            glDevice->glBindBufferBase(target, GLuint(baseIndex + i), binding.buffer.as<GLBuffer>()->handle);
    }
}

void GLShaderResourceBinding::activateSampledImages()
{
    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::SampledImage];
    auto glDevice = device.as<GLDevice> ();

    for (size_t i = 0; i < sampledImages.size(); ++i)
    {
        auto &binding = sampledImages[i];
        if(!binding.texture)
            continue;

        auto id = baseIndex + i;

        auto target = binding.texture.as<GLTexture> ()->target;
        glDevice->glActiveTexture(GLenum(GL_TEXTURE0 + id));
        glBindTexture(target, binding.texture.as<GLTexture>()->handle);
    }
}

void GLShaderResourceBinding::activateSamplers()
{
    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    size_t baseIndex = bank.startIndices[(int)OpenGLResourceBindingType::Sampler];
    auto glDevice = device.as<GLDevice> ();
    for (size_t i = 0; i < samplers.size(); ++i)
    {
        auto sampler = samplers[i];
        if(sampler)
            glDevice->glBindSampler(GLuint(baseIndex + i), sampler);
    }
}

} // End of namespace AgpuGL
