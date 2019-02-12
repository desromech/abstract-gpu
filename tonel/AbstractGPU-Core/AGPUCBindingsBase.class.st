Class {
	#name : #AGPUCBindingsBase,
	#superclass : #FFILibrary,
	#category : #'AbstractGPU-Core'
}

{ #category : #'accessing platform' }
AGPUCBindingsBase >> macModuleName [
	^ 'libAgpu.dylib'
]

{ #category : #'accessing platform' }
AGPUCBindingsBase >> unixModuleName [
	^ 'libAgpu.so'
]

{ #category : #'accessing platform' }
AGPUCBindingsBase >> win32ModuleName [
	^ 'Agpu.dll'
]
