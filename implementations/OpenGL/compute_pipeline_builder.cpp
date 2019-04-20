#include "compute_pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "pipeline_builder.hpp" // For processTextureWithSamplerCombinations
#include "shader.hpp"
#include "shader_signature.hpp"
#include <algorithm>

namespace AgpuGL
{

GLComputePipelineBuilder::GLComputePipelineBuilder(const agpu::device_ref &device)
    : device(device)
{
}

GLComputePipelineBuilder::~GLComputePipelineBuilder()
{
}

agpu::compute_pipeline_builder_ref GLComputePipelineBuilder::create(const agpu::device_ref &device)
{
	return agpu::makeObject<GLComputePipelineBuilder> (device);
}

agpu::pipeline_state_ptr GLComputePipelineBuilder::build ()
{
	if (!shaderSignature)
	{
		errorMessages = "Missing shader signature.";
		return nullptr;
	}

	if (!shader)
	{
		errorMessages = "Missing shader.";
		return nullptr;
	}

	GLuint program = 0;
	std::vector<GLShaderForSignatureRef> shaderInstances;
	std::vector<MappedTextureWithSamplerCombination> mappedTextureWithSamplerCombinations;
	TextureWithSamplerCombinationMap textureWithSamplerCombinationMap;

	buildTextureWithSampleCombinationMapInto(textureWithSamplerCombinationMap, mappedTextureWithSamplerCombinations);

	// Instantiate the shaders
	GLShaderForSignatureRef shaderInstance;
	std::string errorMessage;

	// Create the shader instance.
	auto error = shader.as<GLShader> ()->instanceForSignature(shaderSignature, textureWithSamplerCombinationMap, shaderEntryPointName, &shaderInstance, &errorMessage);
	errorMessages += errorMessage;
	if (error != AGPU_OK)
	{
		printError("Instance error: %d:%s\n", error, errorMessage.c_str());
		return nullptr;
	}

	bool succeded = false;
	deviceForGL->onMainContextBlocking([&] {
		// Create the progrma
		program = deviceForGL->glCreateProgram();

		// Attach the shader instance to the program.
		std::string errorMessage;
		auto error = shaderInstance->attachToProgram(program, &errorMessage);
		errorMessages += errorMessage;
		if (error != AGPU_OK)
			return;

		// Link the program.
		deviceForGL->glLinkProgram(program);

		// Check the link status
		GLint status;
		deviceForGL->glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			// TODO: Get the info log
			return;
		}

		succeded = true;
	});

	if (!succeded)
		return nullptr;

	// Create the pipeline state object
    auto result = agpu::makeObject<GLPipelineState> ();
	auto pipeline = result.as<GLPipelineState> ();
	pipeline->device = device;
	pipeline->programHandle = program;
	pipeline->type = AgpuPipelineStateType::Compute;
	pipeline->shaderSignature = shaderSignature;
	pipeline->shaderInstances = shaderInstances;
	pipeline->mappedTextureWithSamplerCombinations = mappedTextureWithSamplerCombinations;

	return result.disown();
}

agpu_error GLComputePipelineBuilder::attachShader(const agpu::shader_ref &newShader)
{
    CHECK_POINTER(newShader);
    return attachShaderWithEntryPoint(newShader, AGPU_COMPUTE_SHADER, "main");
}

agpu_error GLComputePipelineBuilder::attachShaderWithEntryPoint (const agpu::shader_ref &newShader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(newShader);
    CHECK_POINTER(entry_point);

    if(type != AGPU_COMPUTE_SHADER)
        return AGPU_INVALID_PARAMETER;

    shader = newShader;
    shaderEntryPointName = entry_point;
    return AGPU_OK;

}

agpu_size GLComputePipelineBuilder::getBuildingLogLength (  )
{
	return (agpu_size)errorMessages.size();
}

agpu_error GLComputePipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    if(buffer_size == 0)
        return AGPU_OK;

    size_t toCopy = std::min(size_t(buffer_size - 1), errorMessages.size());
    if(toCopy > 0)
        memcpy(buffer, errorMessages.data(), toCopy);
    buffer[buffer_size-1] = 0;
	return AGPU_OK;
}

agpu_error GLComputePipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &newSignature)
{
    CHECK_POINTER(newSignature);
    this->shaderSignature = newSignature;
    return AGPU_OK;
}

void GLComputePipelineBuilder::buildTextureWithSampleCombinationMapInto(TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations)
{
	std::set<TextureWithSamplerCombination> rawTextureSamplerCombinations;

	// Get all of the combinations.
	for (auto combination : shader.as<GLShader>()->getTextureWithSamplerCombination(shaderEntryPointName))
		rawTextureSamplerCombinations.insert(combination);

	processTextureWithSamplerCombinations(rawTextureSamplerCombinations, shaderSignature, map, usedCombinations);
}

} // End of namespace AgpuGL
