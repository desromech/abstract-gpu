Class {
	#name : #AGPUCommandAllocator,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUCommandAllocator >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_command_allocator: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandAllocator >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_command_allocator: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandAllocator >> reset [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance reset_command_allocator: (self validHandle).
	self checkErrorCode: resultValue_
]

