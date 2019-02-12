Class {
	#name : #AGPUSwapChain,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'wrappers' }
AGPUSwapChain >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_swap_chain: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUSwapChain >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_swap_chain: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUSwapChain >> swapBuffers [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance swapBuffers_swap_chain: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUSwapChain >> getCurrentBackBuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCurrentBackBuffer_swap_chain: (self validHandle).
	^ AGPUFramebuffer forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUSwapChain >> getCurrentBackBufferIndex [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCurrentBackBufferIndex_swap_chain: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUSwapChain >> getFramebufferCount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getFramebufferCount_swap_chain: (self validHandle).
	^ resultValue_
]

