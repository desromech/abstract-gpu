Class {
	#name : #AGPUOfflineShaderCompiler,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_offline_shader_compiler: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_offline_shader_compiler: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> isShaderLanguageSupported: language [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isShaderLanguageSupported_offline_shader_compiler: (self validHandle) language: language.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> isTargetShaderLanguageSupported: language [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isTargetShaderLanguageSupported_offline_shader_compiler: (self validHandle) language: language.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> setShaderSource: language stage: stage sourceText: sourceText sourceTextLength: sourceTextLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setShaderSource_offline_shader_compiler: (self validHandle) language: language stage: stage sourceText: sourceText sourceTextLength: sourceTextLength.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> compileShader: target_language options: options [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance compileShader_offline_shader_compiler: (self validHandle) target_language: target_language options: options.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> getCompilationLogLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCompilationLogLength_offline_shader_compiler: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> getCompilationLog: buffer_size buffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCompilationLog_offline_shader_compiler: (self validHandle) buffer_size: buffer_size buffer: buffer.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> getCompilationResultLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCompilationResultLength_offline_shader_compiler: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> getCompilationResult: buffer_size buffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCompilationResult_offline_shader_compiler: (self validHandle) buffer_size: buffer_size buffer: buffer.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUOfflineShaderCompiler >> getResultAsShader [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getResultAsShader_offline_shader_compiler: (self validHandle).
	^ AGPUShader forHandle: resultValue_
]

