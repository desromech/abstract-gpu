#include "pipeline_state.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include "command_list.hpp"
#include "shader.hpp"
#include "texture.hpp"

void AgpuGraphicsPipelineStateData::activate()
{
	// Activate the srgb framebuffer if we have a srgb render target attached.
	enableState(hasSRGBTarget, GL_FRAMEBUFFER_SRGB);

	// The scissor test is always enabled.
	glEnable(GL_SCISSOR_TEST);

	// Face culling
	glFrontFace(frontFaceWinding);
	if (cullingMode == GL_NONE)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(cullingMode);
	}

	// Depth
	enableState(depthEnabled, GL_DEPTH_TEST);
	glDepthMask(depthWriteMask);
	glDepthFunc(depthFunction);

	// Set the depth range mapping to [0.0, 1.0]. This is the same depth range used by Direct3D.
	if (device->hasExtension_GL_ARB_clip_control)
		device->glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	else if (device->hasExtension_GL_NV_depth_buffer_float)
		device->glDepthRangedNV(-1, 1);
	else
		glDepthRange(-1, 1);

	// Color buffer
	glColorMask(redMask, greenMask, blueMask, alphaMask);
	enableState(blendingEnabled, GL_BLEND);
	if (blendingEnabled)
	{
		device->glBlendEquationSeparate(blendOperation, blendOperationAlpha);
		device->glBlendFuncSeparate(sourceBlendFactor, destBlendFactor, sourceBlendFactorAlpha, destBlendFactorAlpha);
	}

	// Stencil
	enableState(stencilEnabled, GL_STENCIL_TEST);

	if (stencilEnabled)
	{
		glStencilMask(stencilWriteMask);
		updateStencilReference(0);
		device->glStencilOpSeparate(GL_FRONT, stencilFrontFailOp, stencilFrontDepthFailOp, stencilFrontDepthPassOp);
		device->glStencilOpSeparate(GL_BACK, stencilBackFailOp, stencilBackDepthFailOp, stencilBackDepthPassOp);
	}

	// Multisampling
	enableState(sampleCount > 1, GL_MULTISAMPLE);
}

void AgpuGraphicsPipelineStateData::updateStencilReference(int reference)
{
	if (!stencilEnabled)
		return;

	device->glStencilFuncSeparate(GL_FRONT, stencilFrontFunc, reference, stencilReadMask);
	device->glStencilFuncSeparate(GL_BACK, stencilBackFunc, reference, stencilReadMask);
}

void AgpuGraphicsPipelineStateData::enableState(bool enabled, GLenum state)
{
	if (enabled)
		glEnable(state);
	else
		glDisable(state);
}

_agpu_pipeline_state::_agpu_pipeline_state()
{
    shaderSignature = nullptr;
	extraStateData = nullptr;
}

void _agpu_pipeline_state::lostReferences()
{
    if (shaderSignature)
        shaderSignature->release();
    device->onMainContextBlocking([&] {
        device->glDeleteProgram(programHandle);
    });

    for(auto shaderInstance : shaderInstances)
        shaderInstance->release();

	delete extraStateData;
}

agpu_int _agpu_pipeline_state::getUniformLocation ( agpu_cstring name )
{
	return device->glGetUniformLocation(programHandle, name);
}

void _agpu_pipeline_state::activate()
{
	// Use the program.
	device->glUseProgram(programHandle);
	if (extraStateData)
		extraStateData->activate();

}

void _agpu_pipeline_state::activateShaderResourcesOn(CommandListExecutionContext *context, agpu_shader_resource_binding **shaderResourceBindings)
{
    for(size_t i = 0; i < CommandListExecutionContext::MaxNumberOfShaderResourceBindings; ++i)
    {
        auto shaderResource = shaderResourceBindings[i];
        if(shaderResource)
            shaderResource->activate();
    }

    for(auto &combination : mappedTextureWithSamplerCombinations)
    {
        if(combination.combination.textureDescriptorSet >= CommandListExecutionContext::MaxNumberOfShaderResourceBindings)
            continue;

        auto textureShaderResource = shaderResourceBindings[combination.combination.textureDescriptorSet];
        if(!textureShaderResource)
            continue;

        // Get the texture binding.
        auto textureBinding = textureShaderResource->getTextureBindingAt(combination.combination.textureDescriptorBinding);
        if(!textureBinding)
            continue;

        if(combination.combination.samplerDescriptorSet >= CommandListExecutionContext::MaxNumberOfShaderResourceBindings ||
            !shaderResourceBindings[combination.combination.samplerDescriptorSet])
            continue;

        auto samplerShaderResource = shaderResourceBindings[combination.combination.samplerDescriptorSet];
        if(!samplerShaderResource)
            continue;

        // Get the sampler
        auto samplerBinding = samplerShaderResource->getSamplerAt(combination.combination.samplerDescriptorBinding);

        // Activate the texture.
        device->glActiveTexture(GL_TEXTURE0 + combination.mappedTextureUnit);
        glBindTexture(textureBinding->texture->target, textureBinding->texture->handle);

        // Activate the sampler.
        device->glBindSampler(combination.mappedTextureUnit, samplerBinding);
    }
}

void _agpu_pipeline_state::updateStencilReference(int reference)
{
	if (extraStateData)
		extraStateData->updateStencilReference(reference);
}

// C functions
AGPU_EXPORT agpu_error agpuAddPipelineStateReference ( agpu_pipeline_state* pipeline_state )
{
	CHECK_POINTER(pipeline_state);
	pipeline_state->retain();
	return AGPU_OK;
}

AGPU_EXPORT agpu_error agpuReleasePipelineState ( agpu_pipeline_state* pipeline_state )
{
	CHECK_POINTER(pipeline_state);
	pipeline_state->release();
	return AGPU_OK;
}

AGPU_EXPORT agpu_int agpuGetUniformLocation ( agpu_pipeline_state* pipeline_state, agpu_cstring name )
{
	CHECK_POINTER(pipeline_state);
	return pipeline_state->getUniformLocation(name);
}
