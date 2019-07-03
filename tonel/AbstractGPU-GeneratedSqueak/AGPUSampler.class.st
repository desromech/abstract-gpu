Class {
	#name : #AGPUSampler,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUSampler >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_sampler: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUSampler >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_sampler: (self validHandle).
	self checkErrorCode: resultValue_
]

