Class {
	#name : #AGPUPipelineBuilder,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUPipelineBuilder >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_pipeline_builder: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_pipeline_builder: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> build [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance build_pipeline_builder: (self validHandle).
	^ AGPUPipelineState forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> attachShader: shader [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance attachShader_pipeline_builder: (self validHandle) shader: (self validHandleOf: shader).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> attachShaderWithEntryPoint: shader type: type entry_point: entry_point [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance attachShaderWithEntryPoint_pipeline_builder: (self validHandle) shader: (self validHandleOf: shader) type: type entry_point: entry_point.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> getBuildingLogLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getBuildingLogLength_pipeline_builder: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> getBuildingLog: buffer_size buffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getBuildingLog_pipeline_builder: (self validHandle) buffer_size: buffer_size buffer: buffer.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setBlendState: renderTargetMask enabled: enabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setBlendState_pipeline_builder: (self validHandle) renderTargetMask: renderTargetMask enabled: enabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setBlendFunction: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setBlendFunction_pipeline_builder: (self validHandle) renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setColorMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setColorMask_pipeline_builder: (self validHandle) renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setFrontFace: winding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setFrontFace_pipeline_builder: (self validHandle) winding: winding.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setCullMode: mode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setCullMode_pipeline_builder: (self validHandle) mode: mode.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setDepthBias: constant_factor clamp: clamp slope_factor: slope_factor [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthBias_pipeline_builder: (self validHandle) constant_factor: constant_factor clamp: clamp slope_factor: slope_factor.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setDepthState: enabled writeMask: writeMask function: function [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthState_pipeline_builder: (self validHandle) enabled: enabled writeMask: writeMask function: function.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setPolygonMode: mode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setPolygonMode_pipeline_builder: (self validHandle) mode: mode.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setStencilState: enabled writeMask: writeMask readMask: readMask [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilState_pipeline_builder: (self validHandle) enabled: enabled writeMask: writeMask readMask: readMask.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setStencilFrontFace: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilFrontFace_pipeline_builder: (self validHandle) stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setStencilBackFace: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilBackFace_pipeline_builder: (self validHandle) stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setRenderTargetCount: count [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setRenderTargetCount_pipeline_builder: (self validHandle) count: count.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setRenderTargetFormat: index format: format [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setRenderTargetFormat_pipeline_builder: (self validHandle) index: index format: format.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setDepthStencilFormat: format [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthStencilFormat_pipeline_builder: (self validHandle) format: format.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setPrimitiveType: type [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setPrimitiveType_pipeline_builder: (self validHandle) type: type.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setVertexLayout: layout [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setVertexLayout_pipeline_builder: (self validHandle) layout: (self validHandleOf: layout).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setShaderSignature: signature [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setShaderSignature_pipeline_builder: (self validHandle) signature: (self validHandleOf: signature).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineBuilder >> setSampleDescription: sample_count sample_quality: sample_quality [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setSampleDescription_pipeline_builder: (self validHandle) sample_count: sample_count sample_quality: sample_quality.
	self checkErrorCode: resultValue_
]

