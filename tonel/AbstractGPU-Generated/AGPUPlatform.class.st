Class {
	#name : #AGPUPlatform,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-Generated'
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

