#include <string.h>
#include "shader.hpp"

inline GLenum mapShaderType(agpu_shader_type type)
{
	switch(type)
	{
	case AGPU_VERTEX_SHADER: return GL_VERTEX_SHADER;
	case AGPU_FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
	case AGPU_COMPUTE_SHADER: return GL_COMPUTE_SHADER;
	case AGPU_GEOMETRY_SHADER: return GL_GEOMETRY_SHADER;
	case AGPU_TESSELLATION_CONTROL_SHADER: return GL_TESS_CONTROL_SHADER;
	case AGPU_TESSELLATION_EVALUATION_SHADER: return GL_TESS_EVALUATION_SHADER;
	default: abort();
	}
}

_agpu_shader::_agpu_shader()
{
	compiled = false;
}

void _agpu_shader::lostReferences()
{
	device->glDeleteShader(handle);
}

agpu_shader *_agpu_shader::createShader(agpu_device *device, agpu_shader_type type)
{
    // A device is needed.
    if(!device)
        return nullptr;

	auto handle = device->glCreateShader(mapShaderType(type));
	auto shader = new agpu_shader();
	shader->device = device;
	shader->type = type;
	shader->handle = handle;
	return shader;
}

agpu_error _agpu_shader::setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
	// Only support glsl for now.
	if(language != AGPU_SHADER_LANGUAGE_GLSL)
		return AGPU_UNSUPPORTED;

	// Check the source code
	CHECK_POINTER(sourceText);		
	if(!sourceTextLength)
		sourceTextLength = strlen(sourceText);
		
	// Set the shader source
	device->glShaderSource(handle, 1, &sourceText, &sourceTextLength);
	return AGPU_OK;
}

agpu_error _agpu_shader::compileShader(agpu_cstring options)
{
	// Compile the shader.
	device->glCompileShader(handle);
	
	// Get the compile status
	GLint status;
	device->glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	compiled = status == GL_TRUE;
	return compiled ? AGPU_OK : AGPU_COMPILATION_ERROR;
}

agpu_size _agpu_shader::getShaderCompilationLogLength()
{
	GLint length;
	device->glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
	return length;
}

agpu_error _agpu_shader::getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
	device->glGetShaderInfoLog(handle, buffer_size, nullptr, buffer);
	return AGPU_OK;
}

// C Interface
AGPU_EXPORT agpu_error agpuAddShaderReference ( agpu_shader* shader )
{
	CHECK_POINTER(shader);
	return shader->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShader ( agpu_shader* shader )
{
	CHECK_POINTER(shader);
	return shader->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSource ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
{
	CHECK_POINTER(shader);
	return shader->setShaderSource(language, sourceText, sourceTextLength);	
}

AGPU_EXPORT agpu_error agpuCompileShader ( agpu_shader* shader, agpu_cstring options )
{
	CHECK_POINTER(shader);
	return shader->compileShader(options);
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength ( agpu_shader* shader )
{
	CHECK_POINTER(shader);
	return shader->getShaderCompilationLogLength();
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer )
{
	CHECK_POINTER(shader);
	return shader->getShaderCompilationLog(buffer_size, buffer);
}
