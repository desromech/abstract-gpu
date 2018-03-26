#include <regex>
#include <string.h>
#include "shader.hpp"
#include "shader_signature.hpp"
#include "spirv_glsl.hpp"

static int shaderDumpCount = 0;

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

inline spv::ExecutionModel mapExecutionModel(agpu_shader_type type)
{
	switch(type)
	{
	case AGPU_VERTEX_SHADER: return spv::ExecutionModelVertex;
	case AGPU_FRAGMENT_SHADER: return spv::ExecutionModelFragment;
	case AGPU_COMPUTE_SHADER: return spv::ExecutionModelGLCompute;
	case AGPU_GEOMETRY_SHADER: return spv::ExecutionModelGeometry;
	case AGPU_TESSELLATION_CONTROL_SHADER: return spv::ExecutionModelTessellationControl;
	case AGPU_TESSELLATION_EVALUATION_SHADER: return spv::ExecutionModelTessellationEvaluation;
	default: abort();
	}
}

agpu_shader_forSignature::agpu_shader_forSignature()
{
    device = nullptr;
	handle = 0;
	rawSourceLanguage = AGPU_SHADER_LANGUAGE_NONE;
}

agpu_error agpu_shader_forSignature::compile(std::string *errorMessage)
{
	CHECK_POINTER(errorMessage);

	agpu_error result = AGPU_OK;
    device->onMainContextBlocking([&]() {
		// Create the shader
		handle = device->glCreateShader(mapShaderType(type));
		if(!handle)
		{
			result = AGPU_UNSUPPORTED;
			return;
		}

		// Set the shader source
		const GLchar *sourceText = glslSource.data();
		GLint sourceTextLength = GLint(glslSource.size());
        device->glShaderSource(handle, 1, &sourceText, &sourceTextLength);

		// Compile the shader
		device->glCompileShader(handle);

		// Get the compilation status
		GLint status;
		device->glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
		if(status != GL_TRUE)
		{
			result = AGPU_COMPILATION_ERROR;

			GLint infoLogLength;
			device->glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);

			// Get the info log
			auto buffer = new char[infoLogLength];
			GLsizei bufferSize;
			device->glGetShaderInfoLog(handle, infoLogLength, &bufferSize, buffer);
			*errorMessage = "Errors when compiling GLSL shader generated from SpirV:\n";
			*errorMessage += sourceText;
			*errorMessage += "\n";
			*errorMessage += std::string(buffer, buffer + bufferSize);
			delete [] buffer;
			return;
		}
    });

	return result;
}

agpu_error agpu_shader_forSignature::attachToProgram(GLuint programHandle, std::string *errorMessage)
{
	device->glAttachShader(programHandle, handle);
	return AGPU_OK;
}

void agpu_shader_forSignature::lostReferences()
{
	device->onMainContextBlocking([&]() {
		if(handle)
			device->glDeleteShader(handle);
	});
}

_agpu_shader::_agpu_shader()
{
	compiled = false;
	genericShaderInstance = nullptr;
}

void _agpu_shader::lostReferences()
{
	if(genericShaderInstance)
		genericShaderInstance->release();
}

agpu_shader *_agpu_shader::createShader(agpu_device *device, agpu_shader_type type)
{
    // A device is needed.
	if(!device)
		return nullptr;

	auto shader = new agpu_shader();
	shader->device = device;
	shader->type = type;
	return shader;
}

std::vector<TextureWithSamplerCombination> &_agpu_shader::getTextureWithSamplerCombination(const std::string &entryPointName)
{
	auto it = textureWithSamplerCombinations.find(entryPointName);
	if (it != textureWithSamplerCombinations.end())
		return it->second;

	if(rawSourceLanguage == AGPU_SHADER_LANGUAGE_GLSL)
		extractGenericShaderTextureWithSamplerCombinations(entryPointName);
	else if(rawSourceLanguage == AGPU_SHADER_LANGUAGE_SPIR_V)
		extractSpirVTextureWithSamplerCombinations(entryPointName);

	return textureWithSamplerCombinations[entryPointName];
}

void _agpu_shader::extractGenericShaderTextureWithSamplerCombinations(const std::string &entryPointName)
{
	textureWithSamplerCombinations.insert(std::make_pair(entryPointName, std::vector<TextureWithSamplerCombination> ()));
}

void _agpu_shader::extractSpirVTextureWithSamplerCombinations(const std::string &entryPointName)
{
	uint32_t *rawData = reinterpret_cast<uint32_t *> (&rawShaderSource[0]);
	size_t rawDataSize = rawShaderSource.size() / 4;

	spirv_cross::CompilerGLSL glsl(rawData, rawDataSize);

	// Combine the samplers and the images.
	glsl.build_combined_image_samplers();

	textureWithSamplerCombinations.insert(std::make_pair(entryPointName, std::vector<TextureWithSamplerCombination>()));
	auto &dest = textureWithSamplerCombinations[entryPointName];

	// Combined sampler/images
	for(auto &remap : glsl.get_combined_image_samplers())
	{
		TextureWithSamplerCombination combination;
		combination.textureDescriptorSet = glsl.get_decoration(remap.image_id, spv::Decoration::DecorationDescriptorSet);
		combination.textureDescriptorBinding = glsl.get_decoration(remap.image_id, spv::Decoration::DecorationBinding);

		combination.samplerDescriptorSet = glsl.get_decoration(remap.sampler_id, spv::Decoration::DecorationDescriptorSet);
		combination.samplerDescriptorBinding = glsl.get_decoration(remap.sampler_id, spv::Decoration::DecorationBinding);
		dest.push_back(combination);
	}
}

agpu_error _agpu_shader::instanceForSignature(agpu_shader_signature *signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, const std::string &entryPoint, agpu_shader_forSignature **result, std::string *errorMessage)
{
	CHECK_POINTER(result);
	CHECK_POINTER(errorMessage);

	if(rawSourceLanguage == AGPU_SHADER_LANGUAGE_GLSL)
		return getOrCreateGenericShaderInstance(signature, textureWithSamplerCombinationMap, result, errorMessage);
	else if(rawSourceLanguage == AGPU_SHADER_LANGUAGE_SPIR_V)
		return getOrCreateSpirVShaderInstance(signature, textureWithSamplerCombinationMap, entryPoint, result, errorMessage);
	return AGPU_INVALID_OPERATION;
}

agpu_error _agpu_shader::getOrCreateGenericShaderInstance(agpu_shader_signature *signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, agpu_shader_forSignature **result, std::string *errorMessage)
{
	if(genericShaderInstance)
	{
		genericShaderInstance->retain();
		*result = genericShaderInstance;
		return AGPU_OK;
	}

	// Create the shader instance object
	auto shaderInstance = new agpu_shader_forSignature();
	shaderInstance->device = device;
	shaderInstance->type = type;
	shaderInstance->glslSource = std::string((const char*)&rawShaderSource[0], (const char*)&rawShaderSource[rawShaderSource.size()]);

	// Compile the shader instance object.
	auto error = shaderInstance->compile(errorMessage);
	if(error == AGPU_OK)
	{
		// Store the result
		*result = shaderInstance;

		// Keep a copy in the cache
		shaderInstance->retain();
		genericShaderInstance = shaderInstance;
		return AGPU_OK;
	}

	// Release the shader instace.
	shaderInstance->release();
	return error;
}

static agpu_error mapShaderResources(spirv_cross::CompilerGLSL &compiler,
	std::vector<spirv_cross::Resource> &resources, agpu_shader_signature *signature, std::string *errorMessage, agpu_shader_binding_type bindingType)
{
	for(auto &resource : resources)
	{
		unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

		int mappedBinding = signature->mapDescriptorSetAndBinding(bindingType, set, binding);
		if(mappedBinding < 0)
		{
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "Descriptor set %d binding %d is not available in the shader signature. Used by resource: %s", set, binding,
				resource.name.c_str());

			*errorMessage += buffer;
			return AGPU_INVALID_PARAMETER;
		}

		//printf("Resource %s set %d binding %d -> %d\n", resource.name.c_str(), set, binding, mappedBinding);
		compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
		compiler.set_decoration(resource.id, spv::DecorationBinding, mappedBinding);
	}

	return AGPU_OK;
}

agpu_error _agpu_shader::getOrCreateSpirVShaderInstance(agpu_shader_signature *signature, const TextureWithSamplerCombinationMap &textureWithSamplerCombinationMap, const std::string &entryPoint, agpu_shader_forSignature **result, std::string *errorMessage)
{
	char buffer[256];
	uint32_t *rawData = reinterpret_cast<uint32_t *> (&rawShaderSource[0]);
	size_t rawDataSize = rawShaderSource.size() / 4;

	spirv_cross::CompilerGLSL glsl(rawData, rawDataSize);

	glsl.set_entry_point(entryPoint, mapExecutionModel(type));

	// Combine the samplers and the images.
	glsl.build_combined_image_samplers();

	// Modify the resources
	spirv_cross::ShaderResources resources = glsl.get_shader_resources();

	// Combined sampler/images
	for(auto &remap : glsl.get_combined_image_samplers())
	{
		TextureWithSamplerCombination combination;
		combination.textureDescriptorSet = glsl.get_decoration(remap.image_id, spv::Decoration::DecorationDescriptorSet);
		combination.textureDescriptorBinding = glsl.get_decoration(remap.image_id, spv::Decoration::DecorationBinding);
		int textureUnit = signature->mapDescriptorSetAndBinding(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, combination.textureDescriptorSet, combination.textureDescriptorBinding);
		if(textureUnit < 0)
		{
			*errorMessage = "Invalid sampled image descriptor set and binding.";
			return AGPU_INVALID_PARAMETER;
		}

		combination.samplerDescriptorSet = glsl.get_decoration(remap.sampler_id, spv::Decoration::DecorationDescriptorSet);
		combination.samplerDescriptorBinding = glsl.get_decoration(remap.sampler_id, spv::Decoration::DecorationBinding);
		int sampler = signature->mapDescriptorSetAndBinding(AGPU_SHADER_BINDING_TYPE_SAMPLER, combination.samplerDescriptorSet, combination.samplerDescriptorBinding);
		if(sampler < 0)
		{
			*errorMessage = "Invalid sampled image descriptor set and binding.";
			return AGPU_INVALID_PARAMETER;
		}

		auto mappedCombinationIt = textureWithSamplerCombinationMap.find(combination);
		assert(mappedCombinationIt != textureWithSamplerCombinationMap.end());

		auto mappedCombination = mappedCombinationIt->second;
		glsl.set_name(remap.combined_id, mappedCombination.name);
		glsl.set_decoration(remap.combined_id, spv::DecorationBinding, mappedCombination.mappedTextureUnit);
	}

	// Uniform buffers
	auto error = mapShaderResources(glsl, resources.uniform_buffers, signature, errorMessage, AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER);
	if(error) return error;

	// Storage buffers
	error = mapShaderResources(glsl, resources.storage_buffers, signature, errorMessage, AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER);
	if(error) return error;

	// Set some options.
	spirv_cross::CompilerGLSL::Options options;
	options.version = device->glslVersionNumber;
	glsl.set_options(options);

	// Compile the shader.
	std::string compiled;
	try
	{
		compiled = glsl.compile();
	}
	catch(spirv_cross::CompilerError &compileError)
	{
		*errorMessage = "Failed to convert Spir-V into glsl\n";
		*errorMessage += compileError.what();
		return AGPU_COMPILATION_ERROR;
	}
	//printf("Compiled shader:\n%s\n", compiled.c_str());

	// Create the shader instance object
	auto shaderInstance = new agpu_shader_forSignature();
	shaderInstance->device = device;
	shaderInstance->type = type;
	shaderInstance->glslSource = compiled;

	// Compile the shader instance object.
	error = shaderInstance->compile(errorMessage);

	if(device->dumpShaders ||
		(error != AGPU_OK && device->dumpShadersOnError))
	{
		snprintf(buffer, sizeof(buffer), "dump%d.spv", shaderDumpCount);

		FILE *f;
#ifdef _WIN32
		auto error = fopen_s(&f, buffer, "wb");
		if (error) abort();
#else
		f = fopen(buffer, "wb");
#endif
		auto res = fwrite(&rawShaderSource[0], rawShaderSource.size(), 1, f);
		fclose(f);
		(void)res;

		snprintf(buffer, sizeof(buffer), "dump%d.glsl", shaderDumpCount);
#ifdef _WIN32
		error = fopen_s(&f, buffer, "wb");
		if (error) abort();
#else
		f = fopen(buffer, "wb");
#endif
		res = fwrite(compiled.data(), compiled.size(), 1, f);
		fclose(f);
		(void)res;

		++shaderDumpCount;
	}

	if(error == AGPU_OK)
	{
		// Store the result
		*result = shaderInstance;
		return AGPU_OK;
	}

	// Release the shader instance.
	shaderInstance->release();
	return error;
}

agpu_error _agpu_shader::setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
	CHECK_POINTER(sourceText);

	if(language != AGPU_SHADER_LANGUAGE_GLSL && language != AGPU_SHADER_LANGUAGE_SPIR_V)
		return AGPU_UNSUPPORTED;

	if(sourceTextLength < 0)
		sourceTextLength = (agpu_string_length)strlen(sourceText);
	rawSourceLanguage = language;
	rawShaderSource.resize(sourceTextLength);
	memcpy(&rawShaderSource[0], sourceText, sourceTextLength);

	return AGPU_OK;
}

agpu_error _agpu_shader::compileShader(agpu_cstring options)
{
	compiled = true;
	return AGPU_OK;
}

agpu_size _agpu_shader::getShaderCompilationLogLength()
{
	return 1;
}

agpu_error _agpu_shader::getShaderCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
	CHECK_POINTER(buffer);
	if(buffer_size == 0)
		return AGPU_INVALID_PARAMETER;

	*buffer = 0;
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
