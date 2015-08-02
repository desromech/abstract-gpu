#include "pipeline_state.hpp"

_agpu_pipeline_state::_agpu_pipeline_state()
{
	
}

void _agpu_pipeline_state::lostReferences()
{
	device->glDeleteProgram(programHandle);
}

agpu_int _agpu_pipeline_state::getUniformLocation ( agpu_cstring name )
{
	return device->glGetUniformLocation(programHandle, name);
}

void _agpu_pipeline_state::activate()
{
	// Use the program.
	device->glUseProgram(programHandle);
	
	// Depth
	enableState(depthEnabled, GL_DEPTH_TEST);
	glDepthMask(depthWriteMask);
	glDepthFunc(depthFunction);
	
	// Stencil
	enableState(stencilEnabled, GL_STENCIL_TEST);
}

void _agpu_pipeline_state::enableState(bool enabled, GLenum state)
{
	if(enabled)
		glEnable(state);
	else
		glDisable(state);
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
