Class {
	#name : #AGPUVertexLayout,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #wrappers }
AGPUVertexLayout >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_vertex_layout: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUVertexLayout >> addVertexAttributeBindings: vertex_buffer_count vertex_strides: vertex_strides attribute_count: attribute_count attributes: attributes [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addVertexAttributeBindings_vertex_layout: (self validHandle) vertex_buffer_count: vertex_buffer_count vertex_strides: vertex_strides attribute_count: attribute_count attributes: attributes.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUVertexLayout >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_vertex_layout: (self validHandle).
	self checkErrorCode: resultValue_
]
