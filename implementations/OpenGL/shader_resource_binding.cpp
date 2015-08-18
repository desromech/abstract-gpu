#include "shader_resource_binding.hpp"
#include "buffer.hpp"

_agpu_shader_resource_binding::_agpu_shader_resource_binding()
{
}

void _agpu_shader_resource_binding::lostReferences()
{
    for (int i = 0; i < 4; ++i)
    {
        auto &binding = uniformBuffers[i];
        if (binding.buffer)
            binding.buffer->release();
    }
}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_device *device, int bank)
{
	auto binding = new agpu_shader_resource_binding();
	binding->device = device;
	binding->bank = bank;
	return binding;
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
	if(location < 0)
		return AGPU_OK;
	if(location >= 4)
		return AGPU_ERROR;

	if(uniform_buffer)
		uniform_buffer->retain();
	if(uniformBuffers[location].buffer)
		uniformBuffers[location].buffer->release();
 	uniformBuffers[location].buffer = uniform_buffer;

	uniformBuffers[location].range = false;
	return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
	if(location < 0)
		return AGPU_OK;
	if(location >= 4)
		return AGPU_ERROR;

	if(uniform_buffer)
		uniform_buffer->retain();
	if(uniformBuffers[location].buffer)
		uniformBuffers[location].buffer->release();
 	uniformBuffers[location].buffer = uniform_buffer;

	uniformBuffers[location].range = true;
	uniformBuffers[location].offset = offset;
	uniformBuffers[location].size = size;
	return AGPU_OK;
}

void _agpu_shader_resource_binding::activate()
{
	for(int i = 0; i < 4; ++i)
	{
		auto &binding = uniformBuffers[i];
		if(!binding.buffer)
			continue;
		
		if(binding.range)
			device->glBindBufferRange(GL_UNIFORM_BUFFER, bank*4 + i, binding.buffer->handle, binding.offset, binding.size);
		else
			device->glBindBufferBase(GL_UNIFORM_BUFFER, bank*4 + i, binding.buffer->handle);
	}
}

// Exported C interace

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
