Class {
	#name : #AGPUVrGenericEvent,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrGenericEvent class >> fields [
	"
	self defineFields
	"
    ^ #(
		(word1 'ulong')
		(word2 'ulong')
		(word3 'ulong')
		(word4 'ulong')
		(word5 'ulong')
		(word6 'ulong')
		(word7 'ulong')
		(word8 'ulong')
	)
]

