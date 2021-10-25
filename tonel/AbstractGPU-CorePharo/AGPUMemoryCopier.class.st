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

{ #category : #constructing }
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
	| sourceAddress destAddress size |
	sourceAddress := replacement tfPointerAddress + (repStart - 1).
	destAddress := externalAddress + (start - 1).
	size := stop - start + 1.
	LibC memCopy: sourceAddress to: destAddress size: size
]
