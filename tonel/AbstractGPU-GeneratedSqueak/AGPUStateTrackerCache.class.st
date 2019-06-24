Class {
	#name : #AGPUStateTrackerCache,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUStateTrackerCache >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_state_tracker_cache: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUStateTrackerCache >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_state_tracker_cache: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUStateTrackerCache >> createStateTracker: type command_queue: command_queue [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createStateTracker_state_tracker_cache: (self validHandle) type: type command_queue: (self validHandleOf: command_queue).
	^ AGPUStateTracker forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUStateTrackerCache >> createStateTrackerWithCommandAllocator: type command_queue: command_queue command_allocator: command_allocator [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createStateTrackerWithCommandAllocator_state_tracker_cache: (self validHandle) type: type command_queue: (self validHandleOf: command_queue) command_allocator: (self validHandleOf: command_allocator).
	^ AGPUStateTracker forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUStateTrackerCache >> createStateTrackerWithFrameBuffering: type command_queue: command_queue framebuffering_count: framebuffering_count [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createStateTrackerWithFrameBuffering_state_tracker_cache: (self validHandle) type: type command_queue: (self validHandleOf: command_queue) framebuffering_count: framebuffering_count.
	^ AGPUStateTracker forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUStateTrackerCache >> createImmediateRenderer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createImmediateRenderer_state_tracker_cache: (self validHandle).
	^ AGPUImmediateRenderer forHandle: resultValue_
]

