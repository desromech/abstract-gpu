Class {
	#name : #AGPUImmediateRenderer,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUImmediateRenderer >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> beginRendering: state_tracker [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginRendering_immediate_renderer: (self validHandle) state_tracker: (self validHandleOf: state_tracker).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> endRendering [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endRendering_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> beginPrimitives: type [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginPrimitives_immediate_renderer: (self validHandle) type: type.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> endPrimitives [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endPrimitives_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> color: r g: g b: b a: a [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance color_immediate_renderer: (self validHandle) r: r g: g b: b a: a.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> texcoord: x y: y [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance texcoord_immediate_renderer: (self validHandle) x: x y: y.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> normal: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance normal_immediate_renderer: (self validHandle) x: x y: y z: z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> vertex: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance vertex_immediate_renderer: (self validHandle) x: x y: y z: z.
	self checkErrorCode: resultValue_
]

