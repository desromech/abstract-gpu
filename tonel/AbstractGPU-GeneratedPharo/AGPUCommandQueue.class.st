Class {
	#name : #AGPUCommandQueue,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUCommandQueue >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_command_queue: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandQueue >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_command_queue: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandQueue >> addCommandList: command_list [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addCommandList_command_queue: (self validHandle) command_list: (self validHandleOf: command_list).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandQueue >> finishExecution [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance finishExecution_command_queue: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandQueue >> signalFence: fence [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance signalFence_command_queue: (self validHandle) fence: (self validHandleOf: fence).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandQueue >> waitFence: fence [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance waitFence_command_queue: (self validHandle) fence: (self validHandleOf: fence).
	self checkErrorCode: resultValue_
]

