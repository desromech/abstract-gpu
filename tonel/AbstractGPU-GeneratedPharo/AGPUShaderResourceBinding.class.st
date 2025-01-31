Class {
	#name : #AGPUShaderResourceBinding,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_shader_resource_binding: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_shader_resource_binding: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindUniformBuffer: location uniform_buffer: uniform_buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindUniformBuffer_shader_resource_binding: (self validHandle) location: location uniform_buffer: (self validHandleOf: uniform_buffer).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindUniformBufferRange: location uniform_buffer: uniform_buffer offset: offset size: size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindUniformBufferRange_shader_resource_binding: (self validHandle) location: location uniform_buffer: (self validHandleOf: uniform_buffer) offset: offset size: size.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindStorageBuffer: location storage_buffer: storage_buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindStorageBuffer_shader_resource_binding: (self validHandle) location: location storage_buffer: (self validHandleOf: storage_buffer).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindStorageBufferRange: location storage_buffer: storage_buffer offset: offset size: size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindStorageBufferRange_shader_resource_binding: (self validHandle) location: location storage_buffer: (self validHandleOf: storage_buffer) offset: offset size: size.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindSampledTextureView: location view: view [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindSampledTextureView_shader_resource_binding: (self validHandle) location: location view: (self validHandleOf: view).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindArrayOfSampledTextureView: location first_index: first_index count: count views: views [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindArrayOfSampledTextureView_shader_resource_binding: (self validHandle) location: location first_index: first_index count: count views: views.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindStorageImageView: location view: view [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindStorageImageView_shader_resource_binding: (self validHandle) location: location view: (self validHandleOf: view).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindSampler: location sampler: sampler [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindSampler_shader_resource_binding: (self validHandle) location: location sampler: (self validHandleOf: sampler).
	self checkErrorCode: resultValue_
]

