Class {
	#name : #AGPUCBindingsBase,
	#superclass : #PhaNAPILibrary,
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #'library downloading' }
AGPUCBindingsBase >> fetchLibrary [
	PhaNAPIBintray fetchLibrary: 'Agpu' fromUser: 'ronsaldo' repository: 'abstract-gpu' package: 'lib'.

]

{ #category : #'library path' }
AGPUCBindingsBase >> findLibraryPath [
	^ PhaNAPI fullLibraryPathForCurrentPlatform: 'Agpu' package: 'abstract-gpu'
]
