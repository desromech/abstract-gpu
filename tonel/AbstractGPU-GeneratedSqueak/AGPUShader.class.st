Class {
	#name : #AGPUShader,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUShader >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_shader: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShader >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_shader: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShader >> setShaderSource: language sourceText: sourceText sourceTextLength: sourceTextLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setShaderSource_shader: (self validHandle) language: language sourceText: sourceText sourceTextLength: sourceTextLength.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShader >> compileShader: options [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance compileShader_shader: (self validHandle) options: options.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShader >> getCompilationLogLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCompilationLogLength_shader: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUShader >> getCompilationLog: buffer_size buffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCompilationLog_shader: (self validHandle) buffer_size: buffer_size buffer: buffer.
	self checkErrorCode: resultValue_
]

