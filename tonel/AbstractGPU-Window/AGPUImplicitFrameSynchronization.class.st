Class {
	#name : #AGPUImplicitFrameSynchronization,
	#superclass : #Object,
	#instVars : [
		'frameIndex',
		'fences',
		'device',
		'frameBufferingIndex',
		'commandQueue',
		'stateTrackerCache',
		'stateTrackers',
		'currentStateTracker',
		'immediateRenderers'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Window'
}

{ #category : #'instance creation' }
AGPUImplicitFrameSynchronization class >> for: aDevice [
	^ self new initializeWithDevice: aDevice
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> beginFrame [
	currentStateTracker := stateTrackers at: frameBufferingIndex.
	currentStateTracker beginRecordingCommands.
]

{ #category : #accessing }
AGPUImplicitFrameSynchronization >> createImmediateRenderers [
	^ (1 to: 3) collect: [ :i | stateTrackerCache createImmediateRenderer ]
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> finishFrame [
	currentStateTracker endRecordingAndFlushCommands.
	currentStateTracker := nil.
	
	frameIndex := frameIndex + 1.
	frameBufferingIndex := (frameIndex \\ 3) + 1.
]

{ #category : #accessing }
AGPUImplicitFrameSynchronization >> immediateRendererForPass: aPass [
	self assert: currentStateTracker ~~ nil.
	^ (immediateRenderers at: aPass ifAbsentPut: [ self createImmediateRenderers ]) at: frameBufferingIndex.

]

{ #category : #initialization }
AGPUImplicitFrameSynchronization >> initializeWithDevice: aDevice [
	device := aDevice.
	frameIndex := 0.
	frameBufferingIndex := 1.
	
	commandQueue := device getDefaultCommandQueue.
	stateTrackerCache := device createStateTrackerCache: commandQueue.
	
	stateTrackers := (1 to: 3) collect: [ :i |
		stateTrackerCache createStateTracker: AGPU_COMMAND_LIST_TYPE_DIRECT command_queue: commandQueue
	].
	immediateRenderers := IdentityDictionary new.
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> renderFrameWith: aBlock [
	self beginFrame.
	[
		aBlock value: currentStateTracker
	] ensure: [ self finishFrame ].
]

{ #category : #accessing }
AGPUImplicitFrameSynchronization >> stateTrackerCache [
	^ stateTrackerCache
]

{ #category : #accessing }
AGPUImplicitFrameSynchronization >> currentStateTracker [
	^ currentStateTracker
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> synchronizeForFrameReadback: aBlock [
	commandQueue finishExecution.
	aBlock value: 1
]
