#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"

inline GLenum mapCompareFunction(agpu_compare_function function)
{
    switch (function)
    {
    case AGPU_ALWAYS: return GL_ALWAYS;
    case AGPU_NEVER: return GL_NEVER;
    case AGPU_LESS: return GL_LESS;
    case AGPU_LESS_EQUAL: return GL_LEQUAL;
    case AGPU_EQUAL: return GL_EQUAL;
    case AGPU_NOT_EQUAL: return GL_NOTEQUAL;
    case AGPU_GREATER: return GL_GREATER;
    case AGPU_GREATER_EQUAL: return GL_GEQUAL;
    default:
        abort();
    }
}

inline GLenum mapStencilOperation(agpu_stencil_operation operation)
{
    switch (operation)
    {
    case AGPU_KEEP: return GL_KEEP;
    case AGPU_ZERO: return GL_ZERO;
    case AGPU_REPLACE: return GL_REPLACE;
    case AGPU_INVERT: return GL_INVERT;
    case AGPU_INCREASE: return GL_INCR;
    case AGPU_INCREASE_WRAP: return GL_INCR_WRAP;
    case AGPU_DECREASE: return GL_DECR;
    case AGPU_DECREASE_WRAP: return GL_DECR_WRAP;
    default:
        abort();
    }
}

inline GLenum mapBlendFactor(agpu_blending_factor factor, bool alpha)
{
    switch (factor)
    {
    case AGPU_BLENDING_ZERO: return GL_ZERO;
    case AGPU_BLENDING_ONE: return GL_ONE;
    case AGPU_BLENDING_SRC_COLOR: return GL_SRC_COLOR;
    case AGPU_BLENDING_INVERTED_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
    case AGPU_BLENDING_SRC_ALPHA: return GL_SRC_ALPHA;
    case AGPU_BLENDING_INVERTED_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
    case AGPU_BLENDING_DEST_ALPHA: return GL_DST_ALPHA;
    case AGPU_BLENDING_INVERTED_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
    case AGPU_BLENDING_DEST_COLOR: return GL_DST_COLOR;
    case AGPU_BLENDING_INVERTED_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
    case AGPU_BLENDING_SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
    case AGPU_BLENDING_CONSTANT_FACTOR: return alpha ? GL_CONSTANT_ALPHA : GL_CONSTANT_COLOR;
    case AGPU_BLENDING_INVERTED_CONSTANT_FACTOR: return alpha ? GL_ONE_MINUS_CONSTANT_ALPHA : GL_ONE_MINUS_CONSTANT_COLOR;
    case AGPU_BLENDING_SRC_1COLOR: return GL_SRC1_COLOR;
    case AGPU_BLENDING_INVERTED_SRC_1COLOR: return GL_ONE_MINUS_SRC1_COLOR;
    case AGPU_BLENDING_SRC_1ALPHA: return GL_SRC1_ALPHA;
    case AGPU_BLENDING_INVERTED_SRC_1ALPHA: return GL_ONE_MINUS_SRC1_ALPHA;
    default:
        abort();
    }
}

inline GLenum mapBlendOperation(agpu_blending_operation operation)
{
    switch (operation)
    {
    case AGPU_BLENDING_OPERATION_ADD: return GL_FUNC_ADD;
    case AGPU_BLENDING_OPERATION_SUBTRACT: return GL_FUNC_SUBTRACT;
    case AGPU_BLENDING_OPERATION_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
    case AGPU_BLENDING_OPERATION_MIN: // Unsupported
    case AGPU_BLENDING_OPERATION_MAX: // Unsupported
    default:
        abort();
    }
}

_agpu_pipeline_builder::_agpu_pipeline_builder()
{
    programHandle = 0;
    linked = false;
    shaderSignature = nullptr;

    // Depth buffer
    depthEnabled = false;
    depthWriteMask = true;
    depthFunction = AGPU_LESS;

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

    // Miscellaneous
    renderTargetCount = 1;
    primitiveType = AGPU_PRIMITIVE_TYPE_POINT;
}

void _agpu_pipeline_builder::lostReferences()
{
    if (shaderSignature)
        shaderSignature->release();

	if(programHandle && !linked)
    {
        device->onMainContextBlocking([&]{
            device->glDeleteProgram(programHandle);
        });
    }
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
    device->onMainContextBlocking([&]{
    	if(programHandle && !linked)
    		device->glDeleteProgram(programHandle);

    	programHandle = device->glCreateProgram();
    });
	linked = false;
	return AGPU_OK;
}

agpu_pipeline_state* _agpu_pipeline_builder::build ()
{
    device->onMainContextBlocking([&]{
    	// Link the program.
    	device->glLinkProgram(programHandle);

    	// Check the link status
    	GLint status;
    	device->glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
    	linked = status == GL_TRUE;
        if(linked)
        {
            // Set the uniform block bindings
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
            }
        }
    });

	if(!linked)
		return nullptr;

	// Create the pipeline state object
	auto pipeline = new agpu_pipeline_state();
	pipeline->device = device;
	pipeline->programHandle = programHandle;
    pipeline->shaderSignature = shaderSignature;
    if (shaderSignature)
        shaderSignature->retain();

	// Depth state
    pipeline->depthEnabled = depthEnabled;
    pipeline->depthWriteMask = depthWriteMask;
    pipeline->depthFunction = mapCompareFunction(depthFunction);

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
    pipeline->primitiveType = primitiveType;
    pipeline->renderTargetCount = renderTargetCount;

    // Do not own the program.
    this->programHandle = 0;
    this->linked = false;

	return pipeline;
}

agpu_error _agpu_pipeline_builder::attachShader ( agpu_shader* shader )
{
	CHECK_POINTER(shader);
    device->onMainContextBlocking([&]{
    	device->glAttachShader(programHandle, shader->handle);
    	for(auto &locationBinding : shader->attributeBindings )
    		device->glBindAttribLocation(programHandle, locationBinding.location, locationBinding.name.c_str());

        for(auto &binding : shader->samplerBindings )
            samplerBindings.push_back(binding);

        for(auto &binding : shader->uniformBindings )
            uniformBindings.push_back(binding);

    });

	return AGPU_OK;
}

agpu_size _agpu_pipeline_builder::getBuildingLogLength (  )
{
	GLint size;
    device->onMainContextBlocking([&]{
    	device->glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &size);
    });
	return size;
}

agpu_error _agpu_pipeline_builder::getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    device->onMainContextBlocking([&]{
        device->glGetProgramInfoLog(programHandle, (GLsizei)(buffer_size - 1), nullptr, buffer);
    });
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
    renderTargetCount = count;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat(agpu_texture_format format)
{
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType(agpu_primitive_type type)
{
    primitiveType = type;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setVertexLayout(agpu_vertex_layout* layout)
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

AGPU_EXPORT agpu_error agpuSetPrimitiveType(agpu_pipeline_builder* pipeline_builder, agpu_primitive_type type)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuSetVertexLayout(agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setVertexLayout(layout);
}
