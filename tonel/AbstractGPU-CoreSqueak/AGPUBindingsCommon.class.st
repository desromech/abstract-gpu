Class {
	#name : #AGPUBindingsCommon,
	#superclass : #Object,
	#category : #'AbstractGPU-CoreSqueak'
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
		Transcript show: 'Got an error from AGPU: ' , errorCode printString; cr.
		self error: 'Got an error from AGPU.'
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
