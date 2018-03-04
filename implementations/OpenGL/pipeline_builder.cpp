#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "constants.hpp"
#include "../Common/texture_formats_common.hpp"
#include <set>

_agpu_pipeline_builder::_agpu_pipeline_builder()
{
    shaderSignature = nullptr;

    // Depth buffer
    depthEnabled = false;
    depthWriteMask = true;
    depthFunction = AGPU_LESS;

    // Face culling
    frontFaceWinding = AGPU_COUNTER_CLOCKWISE;
    cullingMode = AGPU_CULL_MODE_NONE;

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

void _agpu_pipeline_builder::lostReferences()
{
    if (shaderSignature)
        shaderSignature->release();
    for(auto &shader : shaders)
        shader->release();
}

agpu_pipeline_builder *_agpu_pipeline_builder::createBuilder(agpu_device *device)
{
	auto builder = new agpu_pipeline_builder();
	builder->device = device;
	builder->reset();
	return builder;
}

agpu_error _agpu_pipeline_builder::reset()
{
    for(auto &shader : shaders)
        shader->release();
    shaders.clear();
    errorMessages.clear();
	return AGPU_OK;
}

void _agpu_pipeline_builder::buildTextureWithSampleCombinationMapInto(TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations)
{
    std::set<TextureWithSamplerCombination> rawTextureSamplerCombinations;

    // Get all of the combinations.
    for(auto shader : shaders)
    {
        if(!shader)
            continue;

        for (auto combination : shader->getTextureWithSamplerCombination())
            rawTextureSamplerCombinations.insert(combination);
    }

    // Split between natural pairs, and non-natural pairs.
    std::vector<MappedTextureWithSamplerCombination> naturalTextureWithSamplerCombinations;
    std::vector<MappedTextureWithSamplerCombination> nonNaturalTextureWithSamplerCombinations;
    for(auto & combination : rawTextureSamplerCombinations)
    {
        int textureUnit = shaderSignature->mapDescriptorSetAndBinding(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, combination.textureDescriptorSet, combination.textureDescriptorBinding);
        if(textureUnit < 0)
            return;

        int sampler = shaderSignature->mapDescriptorSetAndBinding(AGPU_SHADER_BINDING_TYPE_SAMPLER, combination.samplerDescriptorSet, combination.samplerDescriptorBinding);
        if(sampler < 0)
            return;

        MappedTextureWithSamplerCombination mappedCombination;
        mappedCombination.combination = combination;
        mappedCombination.name = combination.createName();
        mappedCombination.sourceTextureUnit = textureUnit;
        mappedCombination.sourceSamplerUnit = sampler;

        if(textureUnit == sampler)
            naturalTextureWithSamplerCombinations.push_back(mappedCombination);
        else
            nonNaturalTextureWithSamplerCombinations.push_back(mappedCombination);
    }

    auto naturalTextureUnitCount = shaderSignature->bindingPointsUsed[(int)OpenGLResourceBindingType::Sampler];

    // Assign the natural pairs
    usedCombinations.reserve(naturalTextureWithSamplerCombinations.size() + nonNaturalTextureWithSamplerCombinations.size());
    for(auto &combination : naturalTextureWithSamplerCombinations)
    {
        combination.mappedTextureUnit = combination.mappedSamplerUnit = combination.sourceSamplerUnit;
        usedCombinations.push_back(combination);
    }

    // Assign the non-natural pairs
    auto nextTextureUnit = naturalTextureUnitCount;
    for(auto &combination : nonNaturalTextureWithSamplerCombinations)
    {
        combination.mappedTextureUnit = nextTextureUnit++;
        combination.mappedSamplerUnit = combination.sourceSamplerUnit;
        usedCombinations.push_back(combination);
    }

    for(auto &combination : usedCombinations)
    {
        map.insert(std::make_pair(combination.combination, combination));
    }
}

agpu_pipeline_state* _agpu_pipeline_builder::build ()
{
    GLuint program = 0;
    bool succeded = true;
    std::vector<agpu_shader_forSignature*> shaderInstances;
    std::vector<MappedTextureWithSamplerCombination> mappedTextureWithSamplerCombinations;
    TextureWithSamplerCombinationMap textureWithSamplerCombinationMap;

    if(!shaders.empty())
    {
        buildTextureWithSampleCombinationMapInto(textureWithSamplerCombinationMap, mappedTextureWithSamplerCombinations);

        // Instantiate the shaders
        for(auto shader : shaders)
        {
            if(!shaderSignature)
            {
                errorMessages += "Missing shader signature.";
                succeded = false;
                break;
            }
            agpu_shader_forSignature *shaderForSignature;
            std::string errorMessage;

            // Create the shader instance.
            auto error = shader->instanceForSignature(shaderSignature, textureWithSamplerCombinationMap, &shaderForSignature, &errorMessage);
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
        {
            for(auto instance : shaderInstances)
                instance->release();
            return nullptr;
        }

        succeded = false;
        device->onMainContextBlocking([&]{
            // Create the progrma
            program = device->glCreateProgram();

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
        	device->glLinkProgram(program);

        	// Check the link status
        	GLint status;
        	device->glGetProgramiv(program, GL_LINK_STATUS, &status);
            if(status != GL_TRUE)
            {
                // Get the info log
                return;
            }

            succeded = true;
            {
                /*// Set the uniform block bindings
                for(auto &binding : uniformBindings)
                {
                    auto blockIndex = device->glGetUniformBlockIndex(programHandle, binding.name.c_str());
                    device->glUniformBlockBinding(programHandle, blockIndex, binding.location);
                }

                if(!samplerBindings.empty())
                {
                    device->glUseProgram(programHandle);

                    // Set the sampler bindings.
                    for(auto &binding : samplerBindings)
                    {
                        auto location = device->glGetUniformLocation(programHandle, binding.name.c_str());
                        device->glUniform1i(location, binding.location);

                    }

                    device->glUseProgram(0);
                }*/
            }
        });

    }

	if(!succeded)
    {
        for(auto instance : shaderInstances)
            instance->release();
		return nullptr;
    }

	// Create the pipeline state object
	auto pipeline = new agpu_pipeline_state();
	pipeline->device = device;
	pipeline->programHandle = program;
    pipeline->shaderSignature = shaderSignature;
    pipeline->shaderInstances = shaderInstances;
    pipeline->mappedTextureWithSamplerCombinations = mappedTextureWithSamplerCombinations;
    if (shaderSignature)
        shaderSignature->retain();

	// Depth state
    pipeline->depthEnabled = depthEnabled;
    pipeline->depthWriteMask = depthWriteMask;
    pipeline->depthFunction = mapCompareFunction(depthFunction);

    // Face culling
    pipeline->frontFaceWinding = mapFaceWinding(frontFaceWinding);
    pipeline->cullingMode = mapCullingMode(cullingMode);

    // Color buffer
    pipeline->blendingEnabled = blendingEnabled;
    pipeline->redMask = redMask;
    pipeline->greenMask = greenMask;
    pipeline->blueMask = blueMask;
    pipeline->alphaMask = alphaMask;

    pipeline->sourceBlendFactor = mapBlendFactor(sourceBlendFactor, false);
    pipeline->destBlendFactor = mapBlendFactor(destBlendFactor, false);
    pipeline->blendOperation = mapBlendOperation(blendOperation);
    pipeline->sourceBlendFactorAlpha = mapBlendFactor(sourceBlendFactorAlpha, true);
    pipeline->destBlendFactorAlpha = mapBlendFactor(destBlendFactorAlpha, true);
    pipeline->blendOperationAlpha = mapBlendOperation(blendOperationAlpha);

    // Stencil testing
    pipeline->stencilEnabled = stencilEnabled;
    pipeline->stencilWriteMask = stencilWriteMask;
    pipeline->stencilReadMask = stencilReadMask;

    pipeline->stencilFrontFailOp = mapStencilOperation(stencilFrontFailOp);
    pipeline->stencilFrontDepthFailOp = mapStencilOperation(stencilFrontDepthFailOp);
    pipeline->stencilFrontDepthPassOp = mapStencilOperation(stencilFrontDepthPassOp);
    pipeline->stencilFrontFunc = mapCompareFunction(stencilFrontFunc);

    pipeline->stencilBackFailOp = mapStencilOperation(stencilBackFailOp);
    pipeline->stencilBackDepthFailOp = mapStencilOperation(stencilBackDepthFailOp);
    pipeline->stencilBackDepthPassOp = mapStencilOperation(stencilBackDepthPassOp);
    pipeline->stencilBackFunc = mapCompareFunction(stencilBackFunc);

    // Miscellaneous
    pipeline->primitiveTopology = primitiveType;
    pipeline->renderTargetCount = renderTargetFormats.size();
    pipeline->hasSRGBTarget = false;
    for (auto format : renderTargetFormats)
    {
        if(isSRGBTextureFormat(format))
        {
            pipeline->hasSRGBTarget = true;
            break;
        }

    }

	return pipeline;
}

agpu_error _agpu_pipeline_builder::attachShader ( agpu_shader* shader )
{
	CHECK_POINTER(shader);

    shader->retain();
    shaders.push_back(shader);
	return AGPU_OK;
}

agpu_size _agpu_pipeline_builder::getBuildingLogLength (  )
{
	return errorMessages.size();
}

agpu_error _agpu_pipeline_builder::getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    if(buffer_size == 0)
        return AGPU_OK;

    size_t toCopy = std::min(size_t(buffer_size - 1), errorMessages.size());
    if(toCopy > 0)
        memcpy(buffer, errorMessages.data(), toCopy);
    buffer[buffer_size-1] = 0;
	return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setShaderSignature(agpu_shader_signature* signature)
{
    CHECK_POINTER(signature);

    signature->retain();
    if (shaderSignature)
        shaderSignature->release();
    shaderSignature = signature;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    this->blendingEnabled = enabled;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    this->sourceBlendFactor = sourceFactor;
    this->destBlendFactor = destFactor;
    this->blendOperation = colorOperation;
    this->sourceBlendFactorAlpha = sourceAlphaFactor;
    this->destBlendFactorAlpha = destAlphaFactor;
    this->blendOperationAlpha = alphaOperation;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    this->redMask = redEnabled;
    this->greenMask = greenEnabled;
    this->blueMask = blueEnabled;
    this->alphaMask = alphaEnabled;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setFrontFace ( agpu_face_winding winding )
{
    this->frontFaceWinding = winding;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setCullMode ( agpu_cull_mode mode )
{
    this->cullingMode = mode;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
    this->depthEnabled = enabled;
    this->depthWriteMask = writeMask;
    this->depthFunction = function;
	return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
    this->stencilEnabled = enabled;
    this->stencilWriteMask = writeMask;
    this->stencilReadMask = readMask;
	return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    this->stencilFrontFailOp = stencilFailOperation;
    this->stencilFrontDepthFailOp = depthFailOperation;
    this->stencilFrontDepthPassOp = stencilDepthPassOperation;
    this->stencilFrontFunc = stencilFunction;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    this->stencilBackFailOp = stencilFailOperation;
    this->stencilBackDepthFailOp = depthFailOperation;
    this->stencilBackDepthPassOp = stencilDepthPassOperation;
    this->stencilBackFunc = stencilFunction;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount(agpu_int count)
{
    renderTargetFormats.resize(count, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    if (index >= renderTargetFormats.size())
        return AGPU_INVALID_PARAMETER;

    renderTargetFormats[index] = format;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat(agpu_texture_format format)
{
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType(agpu_primitive_topology type)
{
    primitiveType = type;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setVertexLayout(agpu_vertex_layout* layout)
{
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    return AGPU_OK;
}

// C Interface
AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference ( agpu_pipeline_builder* pipeline_builder )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder ( agpu_pipeline_builder* pipeline_builder )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->release();
}

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature(agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setShaderSignature(signature);
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState ( agpu_pipeline_builder* pipeline_builder )
{
    if (!pipeline_builder)
        return nullptr;
    return pipeline_builder->build();
}

AGPU_EXPORT agpu_error agpuAttachShader ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->attachShader(shader);
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength ( agpu_pipeline_builder* pipeline_builder )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->getBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->getBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetBlendState(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setBlendState(renderTargetMask, enabled);
}

AGPU_EXPORT agpu_error agpuSetBlendFunction(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
}

AGPU_EXPORT agpu_error agpuSetColorMask(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
}

AGPU_EXPORT agpu_error agpuSetFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_face_winding winding )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setFrontFace(winding);
}

AGPU_EXPORT agpu_error agpuSetCullMode ( agpu_pipeline_builder* pipeline_builder, agpu_cull_mode mode )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setCullMode(mode);
}

AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->setDepthState(enabled, writeMask, function);
}

AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->setStencilState(enabled, writeMask, readMask);
}

AGPU_EXPORT agpu_error agpuSetStencilFrontFace(agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetStencilBackFace(agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->setRenderTargetCount(count);

}

AGPU_EXPORT agpu_error agpuSetRenderTargetFormat(agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setRenderTargetFormat(index, format);
}

AGPU_EXPORT agpu_error agpuSetDepthStencilFormat(agpu_pipeline_builder* pipeline_builder, agpu_texture_format format)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setDepthStencilFormat(format);
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType(agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuSetVertexLayout(agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setVertexLayout(layout);
}

AGPU_EXPORT agpu_error agpuSetSampleDescription(agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setSampleDescription(sample_count, sample_quality);
}
