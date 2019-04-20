#include "pipeline_state.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include "command_list.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <algorithm>

namespace AgpuGL
{

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
	if (deviceForGL->hasExtension_GL_ARB_clip_control)
		deviceForGL->glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	else if (deviceForGL->hasExtension_GL_NV_depth_buffer_float)
		deviceForGL->glDepthRangedNV(-1, 1);
	else
		glDepthRange(-1, 1);

	// Color buffer
	glColorMask(redMask, greenMask, blueMask, alphaMask);
	enableState(blendingEnabled, GL_BLEND);
	if (blendingEnabled)
	{
		deviceForGL->glBlendEquationSeparate(blendOperation, blendOperationAlpha);
		deviceForGL->glBlendFuncSeparate(sourceBlendFactor, destBlendFactor, sourceBlendFactorAlpha, destBlendFactorAlpha);
	}

	// Stencil
	enableState(stencilEnabled, GL_STENCIL_TEST);

	if (stencilEnabled)
	{
		glStencilMask(stencilWriteMask);
		updateStencilReference(0);
		deviceForGL->glStencilOpSeparate(GL_FRONT, stencilFrontFailOp, stencilFrontDepthFailOp, stencilFrontDepthPassOp);
		deviceForGL->glStencilOpSeparate(GL_BACK, stencilBackFailOp, stencilBackDepthFailOp, stencilBackDepthPassOp);
	}

	// Multisampling
	enableState(sampleCount > 1, GL_MULTISAMPLE);
}

void AgpuGraphicsPipelineStateData::updateStencilReference(int reference)
{
	if (!stencilEnabled)
		return;

	deviceForGL->glStencilFuncSeparate(GL_FRONT, stencilFrontFunc, reference, stencilReadMask);
	deviceForGL->glStencilFuncSeparate(GL_BACK, stencilBackFunc, reference, stencilReadMask);
}

void AgpuGraphicsPipelineStateData::setBaseInstance(agpu_uint base_instance)
{
	deviceForGL->glUniform1i(baseInstanceUniformIndex, base_instance);
}

void AgpuGraphicsPipelineStateData::enableState(bool enabled, GLenum state)
{
	if (enabled)
		glEnable(state);
	else
		glDisable(state);
}

GLPipelineState::GLPipelineState()
{
	extraStateData = nullptr;
}

GLPipelineState::~GLPipelineState()
{
    deviceForGL->onMainContextBlocking([&] {
        deviceForGL->glDeleteProgram(programHandle);
    });

	delete extraStateData;
}

agpu_int GLPipelineState::getUniformLocation ( agpu_cstring name )
{
	return deviceForGL->glGetUniformLocation(programHandle, name);
}

void GLPipelineState::activate()
{
	// Use the program.
	deviceForGL->glUseProgram(programHandle);
	if (extraStateData)
		extraStateData->activate();
}

void GLPipelineState::activateShaderResourcesOn(CommandListExecutionContext *context, agpu::shader_resource_binding_ref *shaderResourceBindings)
{
    for(size_t i = 0; i < CommandListExecutionContext::MaxNumberOfShaderResourceBindings; ++i)
    {
        const auto &shaderResource = shaderResourceBindings[i];
        if(shaderResource)
            shaderResource.as<GLShaderResourceBinding> ()->activate();
    }

    for(auto &combination : mappedTextureWithSamplerCombinations)
    {
        if(combination.combination.textureDescriptorSet >= CommandListExecutionContext::MaxNumberOfShaderResourceBindings)
            continue;

        const auto &textureShaderResource = shaderResourceBindings[combination.combination.textureDescriptorSet];
        if(!textureShaderResource)
            continue;

        // Get the texture binding.
        const auto &textureBinding = textureShaderResource.as<GLShaderResourceBinding> ()->getTextureBindingAt(combination.combination.textureDescriptorBinding);
        if(!textureBinding)
            continue;

        if(combination.combination.samplerDescriptorSet >= CommandListExecutionContext::MaxNumberOfShaderResourceBindings ||
            !shaderResourceBindings[combination.combination.samplerDescriptorSet])
            continue;

        const auto &samplerShaderResource = shaderResourceBindings[combination.combination.samplerDescriptorSet];
        if(!samplerShaderResource)
            continue;

        // Get the sampler
        auto samplerBinding = samplerShaderResource.as<GLShaderResourceBinding> ()->getSamplerAt(combination.combination.samplerDescriptorBinding);

        // Activate the texture.
        deviceForGL->glActiveTexture(GL_TEXTURE0 + combination.mappedTextureUnit);
        glBindTexture(textureBinding->texture.as<GLTexture>()->target, textureBinding->texture.as<GLTexture>()->handle);

        // Activate the sampler.
        deviceForGL->glBindSampler(combination.mappedTextureUnit, samplerBinding);
    }
}

void GLPipelineState::setBaseInstance(agpu_uint base_instance)
{
	extraStateData->setBaseInstance(base_instance);
}

void GLPipelineState::uploadPushConstants(const uint8_t *pushConstantBuffer, size_t pushConstantBufferSize)
{
	if(!shaderSignature)
		return;

	auto uniformVariableCount = shaderSignature.as<GLShaderSignature> ()->bindingPointsUsed[int(OpenGLResourceBindingType::UniformVariable)];
	if(uniformVariableCount == 0)
		return;

	auto usedBufferSize = std::min(pushConstantBufferSize, size_t(uniformVariableCount*4));
	auto usedUniformCount = usedBufferSize / 4;
	auto sourceData = reinterpret_cast<const int32_t*> (pushConstantBuffer);
	for(size_t i = 0; i < usedUniformCount; ++i)
	{
		// TODO: Support more types than int.
		deviceForGL->glUniform1i(i, sourceData[i]);
	}
}

void GLPipelineState::updateStencilReference(int reference)
{
	if (extraStateData)
		extraStateData->updateStencilReference(reference);
}

} // End of namespace AgpuGL
