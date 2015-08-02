#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"

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
    // Depth buffer
    depthEnabled = false;
    depthWriteMask = true;
    depthFunction = AGPU_LESS;

    // Stencil buffer
    stencilEnabled = false;
    stencilWriteMask = ~0;
    stencilReadMask = ~0;

    // Alpha testing
    alphaTestEnabled = false;
    alphaTestFunction = AGPU_GREATER;

    // Miscellaneos
    renderTargetCount = 1;
    primitiveTopology = AGPU_POINTS;
}

void _agpu_pipeline_builder::lostReferences()
{
	if(programHandle && !linked)
		device->glDeleteProgram(programHandle);
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
	if(programHandle && !linked)
		device->glDeleteProgram(programHandle);
		
	programHandle = device->glCreateProgram();
	linked = false;
	return AGPU_OK;
}

agpu_pipeline_state* _agpu_pipeline_builder::build ()
{
	// Link the program.
	device->glLinkProgram(programHandle);
	
	// Check the link status
	GLint status;
	device->glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
	linked = status == GL_TRUE;
	if(!linked)
		return nullptr;	
	
	// Create the pipeline state object
	auto pipeline = new agpu_pipeline_state();
	pipeline->device = device;
	pipeline->programHandle = programHandle;
	
	// Depth state
    pipeline->depthEnabled = depthEnabled;
    pipeline->depthWriteMask = depthWriteMask;
    pipeline->depthFunction = mapCompareFunction(depthFunction);

    // Stencil testing
    pipeline->stencilEnabled = stencilEnabled;
    pipeline->stencilWriteMask = stencilWriteMask;
    pipeline->stencilReadMask = stencilReadMask;
    
    // Alpha testing
    pipeline->alphaTestEnabled = alphaTestEnabled;
	pipeline->alphaTestFunction = mapCompareFunction(alphaTestFunction);

    // Miscellaneous
    pipeline->primitiveTopology = primitiveTopology;
    pipeline->renderTargetCount = renderTargetCount;

	return pipeline;
}

agpu_error _agpu_pipeline_builder::attachShader ( agpu_shader* shader )
{
	CHECK_POINTER(shader);
	device->glAttachShader(programHandle, shader->handle);
	for(auto &locationBinding : shader->attributeBindings )
		device->glBindAttribLocation(programHandle, locationBinding.location, locationBinding.name.c_str());

	return AGPU_OK;
}

agpu_size _agpu_pipeline_builder::getBuildingLogLength (  )
{
	GLint size;
	device->glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &size);
	return size;
}

agpu_error _agpu_pipeline_builder::getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
	device->glGetProgramInfoLog(programHandle, (GLsizei)buffer_size, nullptr, buffer);
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

agpu_error _agpu_pipeline_builder::setAlphaTestingState ( agpu_bool enable, agpu_compare_function function )
{
	alphaTestEnabled = enable;
	alphaTestFunction = function;
	return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount(agpu_int count)
{
    renderTargetCount = count;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPrimitiveTopology(agpu_primitive_mode topology)
{
    primitiveTopology = topology;
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

AGPU_EXPORT agpu_error agpuSetAlphaTestingState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enable, agpu_compare_function function )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->setAlphaTestingState(enable, function);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count )
{
	CHECK_POINTER(pipeline_builder);
	return pipeline_builder->setRenderTargetCount(count);

}

AGPU_EXPORT agpu_error agpuSetPrimitiveTopology(agpu_pipeline_builder* pipeline_builder, agpu_primitive_mode topology)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPrimitiveTopology(topology);
}
