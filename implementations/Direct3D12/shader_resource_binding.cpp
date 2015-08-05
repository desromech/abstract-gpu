#include "shader_resource_binding.hpp"
#include "buffer.hpp"

_agpu_shader_resource_binding::_agpu_shader_resource_binding()
{
    for (int i = 0; i < 4; ++i)
    {
        constantBuffers[i] = nullptr;
    }
}

void _agpu_shader_resource_binding::lostReferences()
{
    for (int i = 0; i < 4; ++i)
    {
        auto buffer = constantBuffers[i];
        if (buffer)
            buffer->release();
    }

}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_device *device, int bank)
{
    if (bank < 0 || bank >= 4)
        return nullptr;

    auto binding = new agpu_shader_resource_binding();
    binding->device = device;
    binding->bank = bank;
    binding->heapOffset = device->shaderResourceBindingOffsets[bank];
    device->shaderResourceBindingOffsets[bank] += device->shaderResourceViewDescriptorSize * 4;
    return binding;
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->view.constantBuffer.SizeInBytes);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    
    if (uniform_buffer->description.binding != AGPU_UNIFORM_BUFFER)
        return AGPU_ERROR;

    if (location < 0)
        return AGPU_OK;
    if (location >= 4)
        return AGPU_ERROR;

    // Store the uniform buffer
    uniform_buffer->retain();
    if (constantBuffers[location])
        constantBuffers[location]->release();
    constantBuffers[location] = uniform_buffer;

    // Compute the actual view.
    auto view = uniform_buffer->view.constantBuffer;
    view.BufferLocation += offset,
    view.SizeInBytes = (size + 255) & (~255);

    // Set the descriptor location
    auto desc = device->shaderResourcesViewHeaps[bank]->GetCPUDescriptorHandleForHeapStart();
    desc.ptr += heapOffset + location*device->shaderResourceViewDescriptorSize;
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