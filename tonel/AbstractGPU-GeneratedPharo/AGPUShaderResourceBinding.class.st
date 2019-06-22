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
AGPUShaderResourceBinding >> bindTexture: location texture: texture startMiplevel: startMiplevel miplevels: miplevels lodclamp: lodclamp [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindTexture_shader_resource_binding: (self validHandle) location: location texture: (self validHandleOf: texture) startMiplevel: startMiplevel miplevels: miplevels lodclamp: lodclamp.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindTextureArrayRange: location texture: texture startMiplevel: startMiplevel miplevels: miplevels firstElement: firstElement numberOfElements: numberOfElements lodclamp: lodclamp [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindTextureArrayRange_shader_resource_binding: (self validHandle) location: location texture: (self validHandleOf: texture) startMiplevel: startMiplevel miplevels: miplevels firstElement: firstElement numberOfElements: numberOfElements lodclamp: lodclamp.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> bindImage: location texture: texture level: level layer: layer access: access format: format [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindImage_shader_resource_binding: (self validHandle) location: location texture: (self validHandleOf: texture) level: level layer: layer access: access format: format.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderResourceBinding >> createSampler: location description: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createSampler_shader_resource_binding: (self validHandle) location: location description: description.
	self checkErrorCode: resultValue_
]

