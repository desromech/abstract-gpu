#include "compute_pipeline_builder.hpp"
#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include <algorithm>

_agpu_compute_pipeline_builder::_agpu_compute_pipeline_builder(agpu_device *device)
    : device(device)
{
    shader = nullptr;
    shaderSignature = nullptr;
}

void _agpu_compute_pipeline_builder::lostReferences()
{
    if(shader)
        shader->release();
    if(shaderSignature)
        shaderSignature->release();
}

_agpu_compute_pipeline_builder *_agpu_compute_pipeline_builder::create(agpu_device *device)
{
	return new _agpu_compute_pipeline_builder(device);
}

agpu_pipeline_state* _agpu_compute_pipeline_builder::build ()
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
	std::vector<agpu_shader_forSignature*> shaderInstances;
	std::vector<MappedTextureWithSamplerCombination> mappedTextureWithSamplerCombinations;
	TextureWithSamplerCombinationMap textureWithSamplerCombinationMap;
	
	buildTextureWithSampleCombinationMapInto(textureWithSamplerCombinationMap, mappedTextureWithSamplerCombinations);

	// Instantiate the shaders
	agpu_shader_forSignature *shaderInstance;
	std::string errorMessage;

	// Create the shader instance.
	auto error = shader->instanceForSignature(shaderSignature, textureWithSamplerCombinationMap, shaderEntryPointName, &shaderInstance, &errorMessage);
	errorMessages += errorMessage;
	if (error != AGPU_OK)
	{
		printError("Instance error: %d:%s\n", error, errorMessage.c_str());
		return nullptr;
	}

	bool succeded = false;
	device->onMainContextBlocking([&] {
		// Create the progrma
		program = device->glCreateProgram();

		// Attach the shader instance to the program.
		std::string errorMessage;
		auto error = shaderInstance->attachToProgram(program, &errorMessage);
		errorMessages += errorMessage;
		if (error != AGPU_OK)
			return;

		// Link the program.
		device->glLinkProgram(program);

		// Check the link status
		GLint status;
		device->glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			// TODO: Get the info log
			return;
		}

		succeded = true;
	});

	if (!succeded)
	{
		shaderInstance->release();
		return nullptr;
	}

	// Create the pipeline state object
	auto pipeline = new agpu_pipeline_state();
	pipeline->device = device;
	pipeline->programHandle = program;
	pipeline->type = AgpuPipelineStateType::Compute;
	pipeline->shaderSignature = shaderSignature;
	pipeline->shaderInstances = shaderInstances;
	pipeline->mappedTextureWithSamplerCombinations = mappedTextureWithSamplerCombinations;
	if (shaderSignature)
		shaderSignature->retain();

	return pipeline;
}

agpu_error _agpu_compute_pipeline_builder::attachShader ( agpu_shader* newShader )
{
    return attachShaderWithEntryPoint(newShader, "main");
}

agpu_error _agpu_compute_pipeline_builder::attachShaderWithEntryPoint ( agpu_shader* newShader, agpu_cstring entry_point )
{
    CHECK_POINTER(newShader);
    CHECK_POINTER(entry_point);

    newShader->retain();
    if(shader)
        shader->release();
    shader = newShader;
    shaderEntryPointName = entry_point;
    return AGPU_OK;

}

agpu_size _agpu_compute_pipeline_builder::getBuildingLogLength (  )
{
	return (agpu_size)errorMessages.size();
}

agpu_error _agpu_compute_pipeline_builder::getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    if(buffer_size == 0)
        return AGPU_OK;

    size_t toCopy = std::min(size_t(buffer_size - 1), errorMessages.size());
    if(toCopy > 0)
        memcpy(buffer, errorMessages.data(), toCopy);
    buffer[buffer_size-1] = 0;
	return AGPU_OK;
}

agpu_error _agpu_compute_pipeline_builder::setShaderSignature ( agpu_shader_signature* newSignature )
{
    CHECK_POINTER(newSignature);
    newSignature->retain();
    if (this->shaderSignature)
        this->shaderSignature->release();
    this->shaderSignature = newSignature;
    return AGPU_OK;
}

void _agpu_compute_pipeline_builder::buildTextureWithSampleCombinationMapInto(TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations)
{
	std::set<TextureWithSamplerCombination> rawTextureSamplerCombinations;

	// Get all of the combinations.
	for (auto combination : shader->getTextureWithSamplerCombination(shaderEntryPointName))
		rawTextureSamplerCombinations.insert(combination);

	processTextureWithSamplerCombinations(rawTextureSamplerCombinations, shaderSignature, map, usedCombinations);
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddComputePipelineBuilderReference ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseComputePipelineBuilder ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildComputePipelineState ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    if(!compute_pipeline_builder)
        return nullptr;

    return compute_pipeline_builder->build();
}

AGPU_EXPORT agpu_error agpuAttachComputeShader ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShader(shader);
}

AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_cstring entry_point )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShaderWithEntryPoint(shader, entry_point);
}

AGPU_EXPORT agpu_size agpuGetComputePipelineBuildingLogLength ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    if(!compute_pipeline_builder)
        return 0;
    return compute_pipeline_builder->getBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetComputePipelineBuildingLog ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->getBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetComputePipelineShaderSignature ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader_signature* signature )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->setShaderSignature(signature);
}
