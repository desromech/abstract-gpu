Class {
	#name : #AGPUSampleMeshVertex,
	#superclass : #Object,
	#instVars : [
		'position',
		'normal',
		'texcoord',
		'tangent4'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #'as yet unclassified' }
AGPUSampleMeshVertex >> drawWithImmediateRenderer: immediateRenderer [
	immediateRenderer
		normal: normal first y: normal second z: normal third;
		texcoord: texcoord first y: texcoord second;
		vertex: position first y: position second z: position third
]

{ #category : #accessing }
AGPUSampleMeshVertex >> normal [

	^ normal
]

{ #category : #accessing }
AGPUSampleMeshVertex >> normal: anObject [

	normal := anObject
]

{ #category : #accessing }
AGPUSampleMeshVertex >> position [

	^ position
]

{ #category : #accessing }
AGPUSampleMeshVertex >> position: anObject [

	position := anObject
]

{ #category : #accessing }
AGPUSampleMeshVertex >> tangent4 [

	^ tangent4
]

{ #category : #accessing }
AGPUSampleMeshVertex >> tangent4: anObject [

	tangent4 := anObject
]

{ #category : #accessing }
AGPUSampleMeshVertex >> texcoord [

	^ texcoord
]

{ #category : #accessing }
AGPUSampleMeshVertex >> texcoord: anObject [

	texcoord := anObject
]
