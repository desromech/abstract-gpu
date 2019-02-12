Class {
	#name : #AGPUBuffer,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'wrappers' }
AGPUBuffer >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_buffer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_buffer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> mapBuffer: flags [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance mapBuffer_buffer: (self validHandle) flags: flags.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> unmapBuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance unmapBuffer_buffer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> getDescription: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getDescription_buffer: (self validHandle) description: description.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> uploadBufferData: offset size: size data: data [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance uploadBufferData_buffer: (self validHandle) offset: offset size: size data: data.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> readBufferData: offset size: size data: data [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance readBufferData_buffer: (self validHandle) offset: offset size: size data: data.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> flushWholeBuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance flushWholeBuffer_buffer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUBuffer >> invalidateWholeBuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance invalidateWholeBuffer_buffer: (self validHandle).
	self checkErrorCode: resultValue_
]

