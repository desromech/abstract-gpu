"
I am an AGPU utility that encapsulates implicit per-frame synchronization states.
"
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
	#category : #'AbstractGPU-Utility'
}

{ #category : #'instance creation' }
AGPUImplicitFrameSynchronization class >> for: aDevice [
	^ self new initializeWithDevice: aDevice
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> beginFrame [
	| fence |
	(fences at: frameBufferingIndex) waitOnClient.

	currentStateTracker := stateTrackers at: frameBufferingIndex.
	currentStateTracker beginRecordingCommands.
]

{ #category : #accessing }
AGPUImplicitFrameSynchronization >> createImmediateRenderers [
	^ (1 to: 3) collect: [ :i | stateTrackerCache createImmediateRenderer ]
]

{ #category : #accessing }
AGPUImplicitFrameSynchronization >> currentStateTracker [
	^ currentStateTracker
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> finishFrame [
	self finishFrameDoing: []
]

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> finishFrameDoing: aBlock [
	currentStateTracker endRecordingAndFlushCommands.
	currentStateTracker := nil.

	aBlock ensure: [
		commandQueue signalFence: (fences at: frameBufferingIndex).

		frameIndex := frameIndex + 1.
		frameBufferingIndex := (frameIndex \\ 3) + 1.
	].
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

	"Create the fences, and make sure they are signaled."
	fences := (1 to: 3) collect: [ :i |
		| fence |
		fence := device createFence.
		commandQueue signalFence: fence.
		fence
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

{ #category : #rendering }
AGPUImplicitFrameSynchronization >> synchronizeForFrameReadback: aBlock [
	commandQueue finishExecution.
	aBlock value: 1
]
