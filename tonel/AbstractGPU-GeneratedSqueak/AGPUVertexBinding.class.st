Class {
	#name : #AGPUVertexBinding,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUVertexBinding >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_vertex_binding: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVertexBinding >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_vertex_binding: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVertexBinding >> bindVertexBuffers: count vertex_buffers: vertex_buffers [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindVertexBuffers_vertex_binding: (self validHandle) count: count vertex_buffers: vertex_buffers.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVertexBinding >> bindVertexBuffersWithOffsets: count vertex_buffers: vertex_buffers offsets: offsets [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindVertexBuffersWithOffsets_vertex_binding: (self validHandle) count: count vertex_buffers: vertex_buffers offsets: offsets.
	self checkErrorCode: resultValue_
]

