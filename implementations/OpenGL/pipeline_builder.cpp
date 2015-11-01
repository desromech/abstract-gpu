#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"

GLenum mapCompareFunction(agpu_compare_function function)
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

_agpu_pipeline_builder::_agpu_pipeline_builder()
{
    programHandle = 0;
    linked = false;
    shaderSignature = nullptr;

    // Depth buffer
    depthEnabled = false;
    depthWriteMask = true;
    depthFunction = AGPU_LESS;

    // Stencil buffer
    stencilEnabled = false;
    stencilWriteMask = ~0;
    stencilReadMask = ~0;

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

    // Stencil testing
    pipeline->stencilEnabled = stencilEnabled;
    pipeline->stencilWriteMask = stencilWriteMask;
    pipeline->stencilReadMask = stencilReadMask;

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

agpu_error _agpu_pipeline_builder::setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
	depthEnabled = enabled;
	depthWriteMask = writeMask;
	depthFunction = function;
	return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
	stencilEnabled = enabled;
    stencilWriteMask = writeMask;
    stencilReadMask = readMask;
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
