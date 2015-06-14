#include "program.hpp"
#include "shader.hpp"

_agpu_program::_agpu_program()
{
	linked = false;
}

void _agpu_program::lostReferences()
{
	device->glDeleteProgram(handle);
}

agpu_program *_agpu_program::createProgram(agpu_device *device)
{
	if(!device)
		return nullptr;
		
	auto handle = device->glCreateProgram();
	auto program = new agpu_program();
	program->device = device;
	program->handle = handle;
	return program;
}

agpu_error _agpu_program::attachShader(agpu_shader* shader)
{
	CHECK_POINTER(shader);
	device->glAttachShader(handle, shader->handle);
	return AGPU_OK;
}

agpu_error _agpu_program::linkProgram()
{
	// Link the program.
	device->glLinkProgram(handle);
	
	// Check the link status
	GLint status;
	device->glGetProgramiv(handle, GL_LINK_STATUS, &status);
	linked = status == GL_TRUE;
	
	return linked ? AGPU_OK : AGPU_LINKING_ERROR;
}

agpu_size _agpu_program::getProgramLinkingLogLength()
{
	GLint size;
	device->glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &size);
	return size;
}

agpu_error _agpu_program::getProgramLinkingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
	device->glGetProgramInfoLog(handle, buffer_size, nullptr, buffer);
	return AGPU_OK;
}

agpu_error _agpu_program::bindAttributeLocation(agpu_cstring name, agpu_int location)
{
	device->glBindAttribLocation(handle, location, name);
	return AGPU_OK;
}

agpu_int _agpu_program::getUniformLocation ( agpu_cstring name )
{
	return device->glGetUniformLocation(handle, name);
}

// C Interface
AGPU_EXPORT agpu_error agpuAddProgramReference ( agpu_program* program )
{
	CHECK_POINTER(program);
	return program->retain();
}

AGPU_EXPORT agpu_error agpuReleaseProgram ( agpu_program* program )
{
	CHECK_POINTER(program);
	return program->release();
}

AGPU_EXPORT agpu_error agpuAttachShader ( agpu_program* program, agpu_shader* shader )
{
	CHECK_POINTER(program);
	return program->attachShader(shader);
}

AGPU_EXPORT agpu_error agpuLinkProgram ( agpu_program* program )
{
	CHECK_POINTER(program);
	return program->linkProgram();
}

AGPU_EXPORT agpu_size agpuGetProgramLinkingLogLength ( agpu_program* program )
{
	CHECK_POINTER(program);
	return program->getProgramLinkingLogLength();
}

AGPU_EXPORT agpu_error agpuGetProgramLinkingLog ( agpu_program* program, agpu_size buffer_size, agpu_string_buffer buffer )
{
	CHECK_POINTER(program);
	return program->getProgramLinkingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuBindAttributeLocation ( agpu_program* program, agpu_cstring name, agpu_int location )
{
	CHECK_POINTER(program);
	return program->bindAttributeLocation(name, location);
}

AGPU_EXPORT agpu_int agpuGetUniformLocation ( agpu_program* program, agpu_cstring name )
{
	CHECK_POINTER(program);
	return program->getUniformLocation(name);
}