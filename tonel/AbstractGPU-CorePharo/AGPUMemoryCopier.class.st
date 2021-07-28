"
I am an utility for copying bits between buffers.
"
Class {
	#name : #AGPUMemoryCopier,
	#superclass : #Object,
	#instVars : [
		'externalAddress'
	],
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #'constructing' }
AGPUMemoryCopier class >> forExternalAddress: anExternalAddress [
	^ self basicNew externalAddress: anExternalAddress; yourself
]

{ #category : #accessing }
AGPUMemoryCopier >> externalAddress [

	^ externalAddress
]

{ #category : #accessing }
AGPUMemoryCopier >> externalAddress: anExternalAddress [

	externalAddress := anExternalAddress
]

{ #category : #accessing }
AGPUMemoryCopier >> replaceFrom: start to: stop with: replacement startingAt: repStart [
	^ externalAddress replaceFrom: start to: stop with: replacement startingAt: repStart
]
