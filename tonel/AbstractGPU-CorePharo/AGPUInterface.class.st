Class {
	#name : #AGPUInterface,
	#superclass : #AGPUBindingsCommon,
	#instVars : [
		'handle'
	],
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #'instance creation' }
AGPUInterface class >> forHandle: aHandle [
	(aHandle isNil or: [ aHandle isNull ]) ifTrue: [ ^ nil ].
	^ self new handle: aHandle; autoRelease; yourself
]

{ #category : #'instance creation' }
AGPUInterface class >> forManualHandle: aHandle [
	^ self new handle: aHandle
]

{ #category : #finalization }
AGPUInterface >> autoRelease [
	^ self class finalizationRegistry add: self
]

{ #category : #accessing }
AGPUInterface >> finalize [
	(handle isNotNil and: [ handle isNull not ]) ifTrue: [
		self primitiveRelease
	]

]

{ #category : #accessing }
AGPUInterface >> handle [
	^ handle
]

{ #category : #accessing }
AGPUInterface >> handle: aHandle [
	handle := aHandle
]

{ #category : #testing }
AGPUInterface >> isNull [
	^ handle isNil or: [ handle isNull ]
]

{ #category : #finalization }
AGPUInterface >> release [
	self class finalizationRegistry remove: self.
	self primitiveRelease.
	handle := nil.
]

{ #category : #accessing }
AGPUInterface >> validHandle [
	handle ifNil: [ self error: 'Trying to use an object with an invalid handle.' ].
	^ handle
]
