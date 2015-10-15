#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"

_agpu_shader_resource_binding::_agpu_shader_resource_binding()
{
}

void _agpu_shader_resource_binding::lostReferences()
{
    if (signature)
        signature->release();

    for (auto &buffer : buffers)
    {
        if (buffer)
            buffer->release();
    }

    for (auto &texture : textures)
    {
        if (texture)
            texture->release();
    }
}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_shader_signature *signature, agpu_uint element, UINT descriptorOffset)
{
    std::unique_ptr<agpu_shader_resource_binding> binding(new agpu_shader_resource_binding());
    binding->device = signature->device;
    binding->signature = signature;
    signature->retain();
    binding->element= element;
    binding->descriptorOffset = descriptorOffset;

    auto &elementDesc = signature->elementsDescription[element];
    binding->isBank = elementDesc.bank;
    binding->type = elementDesc.type;
    
    binding->buffers.resize(elementDesc.bindingPointCount);
    binding->textures.resize(elementDesc.bindingPointCount);
    return binding.release();
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->view.constantBuffer.SizeInBytes);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    if (type != AGPU_SHADER_BINDING_TYPE_CBV)
        return AGPU_INVALID_OPERATION;

    if (uniform_buffer->description.binding != AGPU_UNIFORM_BUFFER)
        return AGPU_ERROR;

    if (location < 0)
        return AGPU_OK;
    if (location >= (int)buffers.size())
        return AGPU_ERROR;

    // Store the uniform buffer
    uniform_buffer->retain();
    if (buffers[location])
        buffers[location]->release();
    buffers[location] = uniform_buffer;

    // Compute the actual view.
    auto view = uniform_buffer->view.constantBuffer;
    view.BufferLocation += offset,
    view.SizeInBytes = (size + 255) & (~255);

    // Set the descriptor location
    auto desc = signature->shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
    desc.ptr += descriptorOffset;
    device->d3dDevice->CreateConstantBufferView(&view, desc);

    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference(agpu_shader_resource_binding* shader_resource_binding)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding(agpu_shader_resource_binding* shader_resource_binding)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->release();
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBuffer(location, uniform_buffer);
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBufferRange(location, uniform_buffer, offset, size);
}
