"
I am an utility for copying bits between buffers.
"
Class {
	#name : #AGPUMemoryCopier,
	#superclass : #Object,
	#instVars : [
		'alien'
	],
	#category : #'AbstractGPU-CoreSqueak'
}

{ #category : #'constructing' }
AGPUMemoryCopier class >> forExternalAddress: anExternalAddress [
	^ self basicNew externalAddress: anExternalAddress; yourself
]
{ #category : #accessing }
AGPUMemoryCopier >> externalAddress: anExternalAddress [

	alien := Alien forPointer: anExternalAddress asInteger
]

{ #category : #accessing }
AGPUMemoryCopier >> replaceFrom: start to: stop with: replacement startingAt: repStart [
	^ alien replaceFrom: start to: stop with: replacement startingAt: repStart
]
