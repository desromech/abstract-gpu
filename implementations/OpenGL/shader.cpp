#include <regex>
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
    device->onMainContextBlocking([&]() {
        device->glDeleteShader(handle);
    });
}

agpu_shader *_agpu_shader::createShader(agpu_device *device, agpu_shader_type type)
{
    // A device is needed.
    if(!device)
        return nullptr;
	GLuint handle;
    device->onMainContextBlocking([&]() {
        handle = device->glCreateShader(mapShaderType(type));
    });

    if(!handle)
        return nullptr;

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
	if(sourceTextLength < 0)
		sourceTextLength = (agpu_string_length)strlen(sourceText);

    // Parse the AGPU pragmas.
    parseShaderPragmas(sourceText, sourceText + sourceTextLength);

	// Set the shader source
    device->onMainContextBlocking([&]() {
        device->glShaderSource(handle, 1, &sourceText, &sourceTextLength);
    });

	return AGPU_OK;
}

void _agpu_shader::parseShaderPragmas(agpu_string sourceTextBegin, agpu_string sourceTextEnd)
{
    const std::string pattern = "#pragma agpu ";
    std::vector<std::string> parameters;
    std::vector<char> parameterCharacters;

    // Find the pragmas
    std::string source(sourceTextBegin, sourceTextEnd);
    for(size_t pos = source.find(pattern); pos < source.size(); pos = source.find(pattern, pos))
    {
        pos += pattern.size();
        parameters.clear();
        parameterCharacters.clear();

        bool inParameter = false;
        while(pos < source.size())
        {
            char c = source[pos++];
            if(c <= ' ' && inParameter)
            {
                parameters.push_back(std::string(parameterCharacters.begin(), parameterCharacters.end()));
                parameterCharacters.clear();
                inParameter = false;
            }
            else if(c > ' ')
            {
                inParameter = true;
                parameterCharacters.push_back(c);
            }

            if(c == '\r' || c == '\n')
                break;
        }

        if(parameters.empty())
            continue;

        processPragma(parameters);
    }

}

void _agpu_shader::processPragma(std::vector<std::string> &pragma)
{
    auto &name = pragma[0];
    if(name == "attribute_location" && pragma.size() >= 3)
    {
        attributeBindings.push_back(LocationBinding(pragma[1], atoi(pragma[2].c_str())));
    }
    else if(name == "uniform_binding" && pragma.size() >= 3)
    {
        uniformBindings.push_back(LocationBinding(pragma[1], atoi(pragma[2].c_str())));
    }
    else if(name == "sampler_binding" && pragma.size() >= 3)
    {
        samplerBindings.push_back(LocationBinding(pragma[1], atoi(pragma[2].c_str())));
    }
}

agpu_error _agpu_shader::compileShader(agpu_cstring options)
{
    bool compiled;
    device->onMainContextBlocking([&]() {
        // Compile the shader.
        device->glCompileShader(handle);

        // Get the compile status
        GLint status;
        device->glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        compiled = status == GL_TRUE;
    });

	return compiled ? AGPU_OK : AGPU_COMPILATION_ERROR;
}

agpu_size _agpu_shader::getShaderCompilationLogLength()
{
	GLint length;
    device->onMainContextBlocking([&]() {
        device->glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
    });
	return length;
}

agpu_error _agpu_shader::getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    device->onMainContextBlocking([&]() {
        device->glGetShaderInfoLog(handle, (GLsizei)(buffer_size - 1), nullptr, buffer);
    });
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
