Class {
	#name : #AGPUInheritanceInfo,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUInheritanceInfo class >> fields [
	"
	self defineFields
	"
    ^ #(
		(flat 'long')
		(renderpass 'void*')
	)
]

