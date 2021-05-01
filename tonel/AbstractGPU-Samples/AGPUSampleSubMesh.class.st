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
