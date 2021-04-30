Class {
	#name : #AGPUSampleSubMesh,
	#superclass : #Object,
	#instVars : [
		'primitiveTopology',
		'firstIndex',
		'indexCount'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #'as yet unclassified' }
AGPUSampleSubMesh >> drawVertices: vertices indices: indices withImmediateRenderer: immediateRenderer [
	immediateRenderer
		beginPrimitives: primitiveTopology;
		color: 1.0 g: 1.0 b: 1.0 a: 1.0.
	
	firstIndex + 1 to: indexCount do: [ :i |
		(vertices at: (indices at: i) + 1) drawWithImmediateRenderer: immediateRenderer
	].

	immediateRenderer endPrimitives
]

{ #category : #accessing }
AGPUSampleSubMesh >> firstIndex [

	^ firstIndex
]

{ #category : #accessing }
AGPUSampleSubMesh >> firstIndex: anObject [

	firstIndex := anObject
]

{ #category : #accessing }
AGPUSampleSubMesh >> indexCount [

	^ indexCount
]

{ #category : #accessing }
AGPUSampleSubMesh >> indexCount: anObject [

	indexCount := anObject
]

{ #category : #accessing }
AGPUSampleSubMesh >> primitiveTopology [

	^ primitiveTopology
]

{ #category : #accessing }
AGPUSampleSubMesh >> primitiveTopology: anObject [

	primitiveTopology := anObject
]
