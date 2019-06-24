Class {
	#name : #AGPURenderpass,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPURenderpass >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_renderpass: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPURenderpass >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_renderpass: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPURenderpass >> setDepthStencilClearValue: value [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthStencilClearValue_renderpass: (self validHandle) value: value.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPURenderpass >> setColorClearValue: attachment_index value: value [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setColorClearValue_renderpass: (self validHandle) attachment_index: attachment_index value: value.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPURenderpass >> setColorClearValueFrom: attachment_index value: value [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setColorClearValueFrom_renderpass: (self validHandle) attachment_index: attachment_index value: value.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPURenderpass >> getColorAttachmentFormats: color_attachment_count formats: formats [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getColorAttachmentFormats_renderpass: (self validHandle) color_attachment_count: color_attachment_count formats: formats.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPURenderpass >> getDepthStencilAttachmentFormat [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getDepthStencilAttachmentFormat_renderpass: (self validHandle).
	^ resultValue_
]

