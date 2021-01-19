Class {
	#name : #AGPUError,
	#superclass : #Error,
	#instVars : [
		'errorCode'
	],
	#category : #'AbstractGPU-CoreSqueak'
}

{ #category : #accessing }
AGPUError >> errorCode [
	^ errorCode
]

{ #category : #accessing }
AGPUError >> errorCode: anObject [
	errorCode := anObject
]
