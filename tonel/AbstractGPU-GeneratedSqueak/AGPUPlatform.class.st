Class {
	#name : #AGPUPlatform,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUPlatform >> openDevice: openInfo [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance openDevice_platform: (self validHandle) openInfo: openInfo.
	^ AGPUDevice forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getName [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getName_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getGpuCount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getGpuCount_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getGpuName: gpu_index [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getGpuName_platform: (self validHandle) gpu_index: gpu_index.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getGpuDeviceType: gpu_index [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getGpuDeviceType_platform: (self validHandle) gpu_index: gpu_index.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> isFeatureSupportedOnGPU: gpu_index feature: feature [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isFeatureSupportedOnGPU_platform: (self validHandle) gpu_index: gpu_index feature: feature.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getLimitValueOnGPU: gpu_index limit: limit [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getLimitValueOnGPU_platform: (self validHandle) gpu_index: gpu_index limit: limit.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getVersion [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getVersion_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> getImplementationVersion [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getImplementationVersion_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> hasRealMultithreading [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance hasRealMultithreading_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> isNative [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isNative_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> isCrossPlatform [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isCrossPlatform_platform: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUPlatform >> createOfflineShaderCompiler [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createOfflineShaderCompiler_platform: (self validHandle).
	^ AGPUOfflineShaderCompiler forHandle: resultValue_
]

