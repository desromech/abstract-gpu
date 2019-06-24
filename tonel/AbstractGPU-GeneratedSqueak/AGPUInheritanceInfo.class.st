Class {
	#name : #AGPUInheritanceInfo,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
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

