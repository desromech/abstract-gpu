Class {
	#name : #AGPUVrSystem,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'wrappers' }
AGPUVrSystem >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_vr_system: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_vr_system: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getVRSystemName [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getVRSystemName_vr_system: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getNativeHandle [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getNativeHandle_vr_system: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getRecommendedRenderTargetSize: size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getRecommendedRenderTargetSize_vr_system: (self validHandle) size: size.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getEyeToHeadTransform: eye transform: transform [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getEyeToHeadTransform_vr_system: (self validHandle) eye: eye transform: transform.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getProjectionMatrix: eye near_distance: near_distance far_distance: far_distance projection_matrix: projection_matrix [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getProjectionMatrix_vr_system: (self validHandle) eye: eye near_distance: near_distance far_distance: far_distance projection_matrix: projection_matrix.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getProjectionFrustumTangents: eye frustum: frustum [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getProjectionFrustumTangents_vr_system: (self validHandle) eye: eye frustum: frustum.
	self checkErrorCode: resultValue_
]

