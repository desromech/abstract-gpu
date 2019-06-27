Class {
	#name : #AGPUTexture,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUTexture >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_texture: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_texture: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> getDescription: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getDescription_texture: (self validHandle) description: description.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> mapLevel: level arrayIndex: arrayIndex flags: flags region: region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance mapLevel_texture: (self validHandle) level: level arrayIndex: arrayIndex flags: flags region: region.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> unmapLevel [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance unmapLevel_texture: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> readTextureData: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch buffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance readTextureData_texture: (self validHandle) level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch buffer: buffer.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> uploadTextureData: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch data: data [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance uploadTextureData_texture: (self validHandle) level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch data: data.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> uploadTextureSubData: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceSize: sourceSize destRegion: destRegion data: data [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance uploadTextureSubData_texture: (self validHandle) level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceSize: sourceSize destRegion: destRegion data: data.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> discardUploadBuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance discardUploadBuffer_texture: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> discardReadbackBuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance discardReadbackBuffer_texture: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> getFullViewDescription: result [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getFullViewDescription_texture: (self validHandle) result: result.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTexture >> createView: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createView_texture: (self validHandle) description: description.
	^ AGPUTextureView forHandle: resultValue_
]

