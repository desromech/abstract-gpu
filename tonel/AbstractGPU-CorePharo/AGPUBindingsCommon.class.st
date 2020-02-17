Class {
	#name : #AGPUBindingsCommon,
	#superclass : #Object,
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #'as yet unclassified' }
AGPUBindingsCommon class >> checkErrorCode: errorCode [
	errorCode < 0 ifTrue: [
		self error: 'Got an error from AGPU.'
	]
]

{ #category : #accessing }
AGPUBindingsCommon class >> validHandleOf: anObject [
	^ anObject ifNil: [ ExternalAddress null ] ifNotNil: [ anObject validHandle ]
]

{ #category : #'error checking' }
AGPUBindingsCommon >> checkErrorCode: errorCode [
	errorCode < 0 ifTrue: [
		AGPUError new
			errorCode: errorCode;
			signal
	]
]

{ #category : #'freeing resources' }
AGPUBindingsCommon >> release [
	self primitiveRelease.
]

{ #category : #accessing }
AGPUBindingsCommon >> validHandleOf: anObject [
	^ anObject ifNotNil: [ anObject validHandle ]
]
