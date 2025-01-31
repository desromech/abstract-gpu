Class {
	#name : #AGPUVrSystem,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
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

{ #category : #'wrappers' }
AGPUVrSystem >> submitEyeRenderTargets: left_eye right_eye: right_eye [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance submitEyeRenderTargets_vr_system: (self validHandle) left_eye: (self validHandleOf: left_eye) right_eye: (self validHandleOf: right_eye).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> waitAndFetchPoses [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance waitAndFetchPoses_vr_system: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getMaxTrackedDevicePoseCount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getMaxTrackedDevicePoseCount_vr_system: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getCurrentTrackedDevicePoseCount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCurrentTrackedDevicePoseCount_vr_system: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getCurrentTrackedDevicePoseInto: index dest: dest [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCurrentTrackedDevicePoseInto_vr_system: (self validHandle) index: index dest: dest.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getMaxRenderTrackedDevicePoseCount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getMaxRenderTrackedDevicePoseCount_vr_system: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getCurrentRenderTrackedDevicePoseCount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCurrentRenderTrackedDevicePoseCount_vr_system: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getCurrentRenderTrackedDevicePoseInto: index dest: dest [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getCurrentRenderTrackedDevicePoseInto_vr_system: (self validHandle) index: index dest: dest.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getTrackedDeviceRenderModel: index [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getTrackedDeviceRenderModel_vr_system: (self validHandle) index: index.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> getControllerState: index dest: dest [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getControllerState_vr_system: (self validHandle) index: index dest: dest.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUVrSystem >> pollEvent: event [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pollEvent_vr_system: (self validHandle) event: event.
	^ resultValue_
]

