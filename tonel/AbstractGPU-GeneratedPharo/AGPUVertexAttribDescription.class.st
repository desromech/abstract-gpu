Class {
	#name : #AGPUVertexAttribDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BINDING',
		'OFFSET_BUFFER',
		'OFFSET_DIVISOR',
		'OFFSET_FORMAT',
		'OFFSET_OFFSET'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUVertexAttribDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint buffer;
		 agpu_uint binding;
		 agpu_texture_format format;
		 agpu_size offset;
		 agpu_uint divisor;
	)
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> binding [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BINDING
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> binding: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BINDING put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> buffer [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BUFFER
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> buffer: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BUFFER put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> divisor [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_DIVISOR
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> divisor: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_DIVISOR put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> offset [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_OFFSET
]

{ #category : #'accessing - structure variables' }
AGPUVertexAttribDescription >> offset: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_OFFSET put: anObject
]
