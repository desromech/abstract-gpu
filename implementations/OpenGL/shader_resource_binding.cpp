#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
#include "buffer.hpp"
#include "sampler.hpp"
#include "constants.hpp"

namespace AgpuGL
{

GLShaderResourceBinding::GLShaderResourceBinding()
{
}

GLShaderResourceBinding::~GLShaderResourceBinding()
{
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
            binding->sampledTextures.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::SampledImage]);
            binding->storageTextures.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::StorageImage]);
            binding->samplers.resize(bank.elementTypeCounts[(int)OpenGLResourceBindingType::Sampler]);
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

agpu_error GLShaderResourceBinding::bindSampledTextureView(agpu_int location, const agpu::texture_view_ref & view)
{
    CHECK_POINTER(view);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return AGPU_INVALID_OPERATION;

    sampledTextures[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::SampledImage]] = view;
    return AGPU_OK;
}

agpu_error GLShaderResourceBinding::bindStorageImageView(agpu_int location, const agpu::texture_view_ref & view)
{
    CHECK_POINTER(view);
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    storageTextures[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::StorageImage]] = view;
    return AGPU_OK;
}

GLAbstractTextureView *GLShaderResourceBinding::getTextureBindingAt(agpu_int location)
{
    if(location < 0)
        return nullptr;

    const auto &bank = signature.as<GLShaderSignature>()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return nullptr;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return nullptr;

    return sampledTextures[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::SampledImage]].as<GLAbstractTextureView> ();
}

agpu_error GLShaderResourceBinding::bindSampler(agpu_int location, const agpu::sampler_ref & sampler)
{
    if(location < 0)
        return AGPU_OK;

    const auto &bank = signature.as<GLShaderSignature> ()->elements[elementIndex];
    if(location >= (agpu_int)bank.elements.size())
        return AGPU_OUT_OF_BOUNDS;

    const auto &element = bank.elements[location];
    if(element.type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    samplers[element.startIndex - bank.startIndices[(int)OpenGLResourceBindingType::Sampler]] = sampler;
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
    for (size_t i = 0; i < sampledTextures.size(); ++i)
    {
        auto &texture = sampledTextures[i];
        auto id = baseIndex + i;

        texture.as<GLAbstractTextureView> ()->activateInSampledSlot(id);
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
