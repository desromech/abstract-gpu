"
I provide the base glue code for the generated C API bindings.
"
Class {
	#name : #AGPUCBindingsBase,
	#superclass : #PhaNAPILibrary,
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #'library downloading' }
AGPUCBindingsBase >> fetchLibrary [
	PhaNAPIGitHubRelease fetchLibrary: 'Agpu' fromIcebergRepoOfClass: self class

]

{ #category : #'library path' }
AGPUCBindingsBase >> findLibraryPath [
	^ PhaNAPI fullLibraryPathForCurrentPlatform: 'Agpu' package: 'abstract-gpu'
]
