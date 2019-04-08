#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "constants.hpp"
#include "../Common/texture_formats_common.hpp"
#include <set>
#include <algorithm>

namespace AgpuGL
{

void processTextureWithSamplerCombinations(const std::set<TextureWithSamplerCombination> &rawTextureSamplerCombinations, const agpu::shader_signature_ref &shaderSignature, TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations)
{
	// Split between natural pairs, and non-natural pairs.
	std::vector<MappedTextureWithSamplerCombination> naturalTextureWithSamplerCombinations;
	std::vector<MappedTextureWithSamplerCombination> nonNaturalTextureWithSamplerCombinations;
	auto glShaderSignature = shaderSignature.as<GLShaderSignature> ();
	for (auto & combination : rawTextureSamplerCombinations)
	{
		int textureUnit = glShaderSignature->mapDescriptorSetAndBinding(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, combination.textureDescriptorSet, combination.textureDescriptorBinding);
		if (textureUnit < 0)
			return;

		int sampler = glShaderSignature->mapDescriptorSetAndBinding(AGPU_SHADER_BINDING_TYPE_SAMPLER, combination.samplerDescriptorSet, combination.samplerDescriptorBinding);
		if (sampler < 0)
			return;

		MappedTextureWithSamplerCombination mappedCombination;
		mappedCombination.combination = combination;
		mappedCombination.name = combination.createName();
		mappedCombination.sourceTextureUnit = textureUnit;
		mappedCombination.sourceSamplerUnit = sampler;

		if (textureUnit == sampler)
			naturalTextureWithSamplerCombinations.push_back(mappedCombination);
		else
			nonNaturalTextureWithSamplerCombinations.push_back(mappedCombination);
	}

	auto naturalTextureUnitCount = glShaderSignature->bindingPointsUsed[(int)OpenGLResourceBindingType::Sampler];

	// Assign the natural pairs
	usedCombinations.reserve(naturalTextureWithSamplerCombinations.size() + nonNaturalTextureWithSamplerCombinations.size());
	for (auto &combination : naturalTextureWithSamplerCombinations)
	{
		combination.mappedTextureUnit = combination.mappedSamplerUnit = combination.sourceSamplerUnit;
		usedCombinations.push_back(combination);
	}

	// Assign the non-natural pairs
	auto nextTextureUnit = naturalTextureUnitCount;
	for (auto &combination : nonNaturalTextureWithSamplerCombinations)
	{
		combination.mappedTextureUnit = nextTextureUnit++;
		combination.mappedSamplerUnit = combination.sourceSamplerUnit;
		usedCombinations.push_back(combination);
	}

	for (auto &combination : usedCombinations)
	{
		map.insert(std::make_pair(combination.combination, combination));
	}
}

GLGraphicsPipelineBuilder::GLGraphicsPipelineBuilder()
{
    // Depth buffer
    depthEnabled = false;
    depthWriteMask = true;
    depthFunction = AGPU_LESS;

    // Depth biasing
    depthBiasEnabled = false;
    depthBiasConstantFactor = 0;
    depthBiasClamp = 0;
    depthBiasSlopeFactor = 0;

    // Face culling
    frontFaceWinding = AGPU_COUNTER_CLOCKWISE;
    cullingMode = AGPU_CULL_MODE_NONE;

    // Polgons
    polygonMode = AGPU_POLYGON_MODE_FILL;

    // Color buffer
    blendingEnabled = false;
    redMask = true;
    greenMask = true;
    blueMask = true;
    alphaMask = true;
    sourceBlendFactor = AGPU_BLENDING_ONE;
    destBlendFactor = AGPU_BLENDING_ZERO;
    blendOperation = AGPU_BLENDING_OPERATION_ADD;
    sourceBlendFactorAlpha = AGPU_BLENDING_ONE;
    destBlendFactorAlpha = AGPU_BLENDING_ZERO;
    blendOperationAlpha = AGPU_BLENDING_OPERATION_ADD;

    // Stencil buffer
    stencilEnabled = false;
    stencilWriteMask = ~0;
    stencilReadMask = ~0;

    stencilFrontFailOp = AGPU_KEEP;
    stencilFrontDepthFailOp = AGPU_KEEP;
    stencilFrontDepthPassOp = AGPU_KEEP;
    stencilFrontFunc = AGPU_ALWAYS;

    stencilBackFailOp = AGPU_KEEP;
    stencilBackDepthFailOp = AGPU_KEEP;
    stencilBackDepthPassOp = AGPU_KEEP;
    stencilBackFunc = AGPU_ALWAYS;

    // Render targets
    renderTargetFormats.resize(1, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM);
    depthStencilFormat = AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT;

    primitiveType = AGPU_POINTS;
}

GLGraphicsPipelineBuilder::~GLGraphicsPipelineBuilder()
{
}

agpu::pipeline_builder_ref GLGraphicsPipelineBuilder::createBuilder(const agpu::device_ref &device)
{
	auto result = agpu::makeObject<GLGraphicsPipelineBuilder> ();
	auto builder = result.as<GLGraphicsPipelineBuilder> ();
	builder->device = device;
	builder->reset();
	return result;
}

agpu_error GLGraphicsPipelineBuilder::reset()
{
    shaders.clear();
    errorMessages.clear();
	return AGPU_OK;
}

void GLGraphicsPipelineBuilder::buildTextureWithSampleCombinationMapInto(TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations)
{
    std::set<TextureWithSamplerCombination> rawTextureSamplerCombinations;

    // Get all of the combinations.
    for(auto &shaderWithEntryPoint : shaders)
    {
        auto shader = shaderWithEntryPoint.first;
        if(!shader)
            continue;

        for (auto combination : shader.as<GLShader>()->getTextureWithSamplerCombination(shaderWithEntryPoint.second))
            rawTextureSamplerCombinations.insert(combination);
    }

	processTextureWithSamplerCombinations(rawTextureSamplerCombinations, shaderSignature, map, usedCombinations);
}

agpu::pipeline_state_ptr GLGraphicsPipelineBuilder::build ()
{
    GLuint program = 0;
	GLint baseInstanceUniformIndex = -1;
    bool succeded = true;
    std::vector<GLShaderForSignatureRef> shaderInstances;
    std::vector<MappedTextureWithSamplerCombination> mappedTextureWithSamplerCombinations;
    TextureWithSamplerCombinationMap textureWithSamplerCombinationMap;

    if(!shaders.empty())
    {
        buildTextureWithSampleCombinationMapInto(textureWithSamplerCombinationMap, mappedTextureWithSamplerCombinations);

        // Instantiate the shaders
        for(auto &shaderWithEntryPoint : shaders)
        {
            const auto &shader = shaderWithEntryPoint.first;
            if(!shaderSignature)
            {
                errorMessages += "Missing shader signature.";
                succeded = false;
                break;
            }
            GLShaderForSignatureRef shaderForSignature;
            std::string errorMessage;

            // Create the shader instance.
            auto error = shader.as<GLShader> ()->instanceForSignature(shaderSignature, textureWithSamplerCombinationMap, shaderWithEntryPoint.second, &shaderForSignature, &errorMessage);
            errorMessages += errorMessage;
            if(error != AGPU_OK)
            {
                printError("Instance error: %d:%s\n", error, errorMessage.c_str());
                succeded = false;
                break;
            }

            shaderInstances.push_back(shaderForSignature);
        }

        if(!succeded)
            return nullptr;

        succeded = false;
        deviceForGL->onMainContextBlocking([&]{
            // Create the progrma
            program = deviceForGL->glCreateProgram();

            // Attach the shaders.
            for(auto shaderInstance : shaderInstances)
            {
                // Attach the shader instance to the program.
                std::string errorMessage;
                auto error = shaderInstance->attachToProgram(program, &errorMessage);

                errorMessages += errorMessage;
                if(error != AGPU_OK)
                    return;
            }

        	// Link the program.
        	deviceForGL->glLinkProgram(program);

        	// Check the link status
        	GLint status;
        	deviceForGL->glGetProgramiv(program, GL_LINK_STATUS, &status);
            if(status != GL_TRUE)
            {
				// TODO: Get the info log
                return;
            }

			// Get some special uniforms
			baseInstanceUniformIndex = deviceForGL->glGetUniformLocation(program, "SPIRV_Cross_BaseInstance");

            succeded = true;
        });
    }

	if(!succeded)
		return nullptr;

	// Create the pipeline state object
	auto result = agpu::makeObject<GLPipelineState> ();
	auto pipeline = result.as<GLPipelineState> ();
	pipeline->device = device;
	pipeline->programHandle = program;
	pipeline->type = AgpuPipelineStateType::Graphics;
    pipeline->shaderSignature = shaderSignature;
    pipeline->shaderInstances = shaderInstances;
    pipeline->mappedTextureWithSamplerCombinations = mappedTextureWithSamplerCombinations;

	auto graphicsState = new AgpuGraphicsPipelineStateData();
	graphicsState->device = device;
	pipeline->extraStateData = graphicsState;

	// Base instance
	graphicsState->baseInstanceUniformIndex = baseInstanceUniformIndex;

	// Depth state
    graphicsState->depthEnabled = depthEnabled;
    graphicsState->depthWriteMask = depthWriteMask;
    graphicsState->depthFunction = mapCompareFunction(depthFunction);

    // Face culling
    graphicsState->frontFaceWinding = mapFaceWinding(frontFaceWinding);
    graphicsState->cullingMode = mapCullingMode(cullingMode);

    // Color buffer
    graphicsState->blendingEnabled = blendingEnabled;
    graphicsState->redMask = redMask;
    graphicsState->greenMask = greenMask;
    graphicsState->blueMask = blueMask;
    graphicsState->alphaMask = alphaMask;

    graphicsState->sourceBlendFactor = mapBlendFactor(sourceBlendFactor, false);
    graphicsState->destBlendFactor = mapBlendFactor(destBlendFactor, false);
    graphicsState->blendOperation = mapBlendOperation(blendOperation);
    graphicsState->sourceBlendFactorAlpha = mapBlendFactor(sourceBlendFactorAlpha, true);
    graphicsState->destBlendFactorAlpha = mapBlendFactor(destBlendFactorAlpha, true);
    graphicsState->blendOperationAlpha = mapBlendOperation(blendOperationAlpha);

    // Stencil testing
    graphicsState->stencilEnabled = stencilEnabled;
    graphicsState->stencilWriteMask = stencilWriteMask;
    graphicsState->stencilReadMask = stencilReadMask;

    graphicsState->stencilFrontFailOp = mapStencilOperation(stencilFrontFailOp);
    graphicsState->stencilFrontDepthFailOp = mapStencilOperation(stencilFrontDepthFailOp);
    graphicsState->stencilFrontDepthPassOp = mapStencilOperation(stencilFrontDepthPassOp);
    graphicsState->stencilFrontFunc = mapCompareFunction(stencilFrontFunc);

    graphicsState->stencilBackFailOp = mapStencilOperation(stencilBackFailOp);
    graphicsState->stencilBackDepthFailOp = mapStencilOperation(stencilBackDepthFailOp);
    graphicsState->stencilBackDepthPassOp = mapStencilOperation(stencilBackDepthPassOp);
    graphicsState->stencilBackFunc = mapCompareFunction(stencilBackFunc);

	// Multisampling
    graphicsState->sampleCount = sampleCount;
    graphicsState->sampleQuality = sampleQuality;

    // Miscellaneous
    graphicsState->primitiveTopology = primitiveType;
    graphicsState->renderTargetCount = (int)renderTargetFormats.size();
    graphicsState->hasSRGBTarget = false;
    for (auto format : renderTargetFormats)
    {
        if(isSRGBTextureFormat(format))
        {
            graphicsState->hasSRGBTarget = true;
            break;
        }

    }

	return result.disown();
}

agpu_error GLGraphicsPipelineBuilder::attachShader(const agpu::shader_ref &shader )
{
	CHECK_POINTER(shader);
	return attachShaderWithEntryPoint(shader, shader.as<GLShader> ()->type, "main");
}

agpu_error GLGraphicsPipelineBuilder::attachShaderWithEntryPoint(const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point )
{
	CHECK_POINTER(shader);

    shaders.push_back(std::make_pair(shader, entry_point));
	return AGPU_OK;
}

agpu_size GLGraphicsPipelineBuilder::getBuildingLogLength (  )
{
	return (agpu_size)errorMessages.size();
}

agpu_error GLGraphicsPipelineBuilder::getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    if(buffer_size == 0)
        return AGPU_OK;

    size_t toCopy = std::min(size_t(buffer_size - 1), errorMessages.size());
    if(toCopy > 0)
        memcpy(buffer, errorMessages.data(), toCopy);
    buffer[buffer_size-1] = 0;
	return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    CHECK_POINTER(signature);
    shaderSignature = signature;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    this->blendingEnabled = enabled;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    this->sourceBlendFactor = sourceFactor;
    this->destBlendFactor = destFactor;
    this->blendOperation = colorOperation;
    this->sourceBlendFactorAlpha = sourceAlphaFactor;
    this->destBlendFactorAlpha = destAlphaFactor;
    this->blendOperationAlpha = alphaOperation;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    this->redMask = redEnabled;
    this->greenMask = greenEnabled;
    this->blueMask = blueEnabled;
    this->alphaMask = alphaEnabled;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setFrontFace ( agpu_face_winding winding )
{
    this->frontFaceWinding = winding;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setCullMode ( agpu_cull_mode mode )
{
    this->cullingMode = mode;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setDepthBias ( agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
    this->depthBiasEnabled = true;
    this->depthBiasConstantFactor = constant_factor;
    this->depthBiasClamp = clamp;
    this->depthBiasSlopeFactor = slope_factor;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
    this->depthEnabled = enabled;
    this->depthWriteMask = writeMask;
    this->depthFunction = function;
	return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
    this->stencilEnabled = enabled;
    this->stencilWriteMask = writeMask;
    this->stencilReadMask = readMask;
	return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    this->stencilFrontFailOp = stencilFailOperation;
    this->stencilFrontDepthFailOp = depthFailOperation;
    this->stencilFrontDepthPassOp = stencilDepthPassOperation;
    this->stencilFrontFunc = stencilFunction;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    this->stencilBackFailOp = stencilFailOperation;
    this->stencilBackDepthFailOp = depthFailOperation;
    this->stencilBackDepthPassOp = stencilDepthPassOperation;
    this->stencilBackFunc = stencilFunction;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setRenderTargetCount(agpu_int count)
{
    renderTargetFormats.resize(count, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM);
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    if (index >= renderTargetFormats.size())
        return AGPU_INVALID_PARAMETER;

    renderTargetFormats[index] = format;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setDepthStencilFormat(agpu_texture_format format)
{
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setPolygonMode(agpu_polygon_mode mode)
{
    this->polygonMode = mode;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setPrimitiveType(agpu_primitive_topology type)
{
    this->primitiveType = type;
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setVertexLayout(const agpu::vertex_layout_ref &layout)
{
    return AGPU_OK;
}

agpu_error GLGraphicsPipelineBuilder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
	this->sampleCount = sample_count;
	this->sampleQuality = sample_quality;
    return AGPU_OK;
}

} // End of namespace AgpuGL
