Class {
	#name : #AGPUCBindingsBase,
	#superclass : #ExternalLibrary,
	#classInstVars : [
		'uniqueInstance'
	],
	#category : #'AbstractGPU-CoreSqueak'
}

{ #category : #'as yet unclassified' }
AGPUCBindingsBase class >> moduleName [
	^ 'Agpu'
]

{ #category : #'as yet unclassified' }
AGPUCBindingsBase class >> uniqueInstance [
	^ uniqueInstance ifNil: [uniqueInstance := self new]
]
