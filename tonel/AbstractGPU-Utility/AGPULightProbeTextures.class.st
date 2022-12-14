"
I hold a collection of textures corresponding to a specific light-probe.
"
Class {
	#name : #AGPULightProbeTextures,
	#superclass : #Object,
	#instVars : [
		'environmentMap',
		'environmentMapSize',
		'diffuseLightProbe',
		'diffuseLightProbeSize',
		'specularLightProbe',
		'specularLightProbeSize'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #inspecting }
AGPULightProbeTextures >> computeMiplevelSizesFor: size [
	^ Array streamContents: [ :out |
		| currentSize|
		currentSize := size.
		[ 
			out nextPut: currentSize.
			currentSize := currentSize // 2.
		] doWhileTrue: [ currentSize >= 1 ]
	].
]

{ #category : #accessing }
AGPULightProbeTextures >> diffuseLightProbe [

	^ diffuseLightProbe
]

{ #category : #accessing }
AGPULightProbeTextures >> diffuseLightProbe: anObject [

	diffuseLightProbe := anObject
]

{ #category : #accessing }
AGPULightProbeTextures >> diffuseLightProbeSize [

	^ diffuseLightProbeSize
]

{ #category : #accessing }
AGPULightProbeTextures >> diffuseLightProbeSize: anObject [

	diffuseLightProbeSize := anObject
]

{ #category : #'as yet unclassified' }
AGPULightProbeTextures >> discardEnvironmentMap [
	environmentMap := nil.
	environmentMapSize := nil.
]

{ #category : #accessing }
AGPULightProbeTextures >> environmentMap [

	^ environmentMap
]

{ #category : #accessing }
AGPULightProbeTextures >> environmentMap: anObject [

	environmentMap := anObject
]

{ #category : #accessing }
AGPULightProbeTextures >> environmentMapSize [

	^ environmentMapSize
]

{ #category : #accessing }
AGPULightProbeTextures >> environmentMapSize: anObject [

	environmentMapSize := anObject
]

{ #category : #inspecting }
AGPULightProbeTextures >> readDiffuseLightProbeForms [
	^ (0 to: 5) collect: [ :i |
		self readTexture: diffuseLightProbe size: diffuseLightProbeSize face: i level: 0
	].
]

{ #category : #inspecting }
AGPULightProbeTextures >> readEnvironmentMapForms [
	^ (0 to: 5) collect: [ :i |
		self readTexture: environmentMap size: environmentMapSize face: i level: 0
	].
]

{ #category : #inspecting }
AGPULightProbeTextures >> readSpecularLightProbeForms [
	| sizes |
	sizes := AGPUTextureLightprobePreprocessor computeMiplevelSizesFor: specularLightProbeSize.

	^ sizes collectWithIndex: [ :levelSize :levelIndex |
		(0 to: 5) collect: [ :face |
			self readTexture: specularLightProbe size: levelSize face: face level: levelIndex - 1
		].
	].
]

{ #category : #inspecting }
AGPULightProbeTextures >> readTexture: texture size: faceSize face: faceIndex level: level [
	| readBuffer decodedBuffer sourceIndex destIndex |
	
	readBuffer := ByteArray new: faceSize*faceSize*8.
	
	texture readTextureData: level arrayIndex: faceIndex pitch: faceSize*8 slicePitch: faceSize*8*faceSize buffer: readBuffer.
	
	decodedBuffer := ByteArray new: faceSize*faceSize*4.
	sourceIndex := 1.
	destIndex := 1.
	1 to: faceSize*faceSize do: [ :j |
		| f |
		f := Float fromIEEE16Bit: (readBuffer unsignedShortAt: sourceIndex).
		decodedBuffer at: destIndex + 2 put: ((f*255 + 0.5) min: 255 max: 0) asInteger.

		f := Float fromIEEE16Bit: (readBuffer unsignedShortAt: sourceIndex + 2).
		decodedBuffer at: destIndex + 1 put: ((f*255 + 0.5) min: 255 max: 0) asInteger.

		f := Float fromIEEE16Bit: (readBuffer unsignedShortAt: sourceIndex + 4).
		decodedBuffer at: destIndex put: ((f*255 + 0.5) min: 255 max: 0) asInteger.

		f := Float fromIEEE16Bit: (readBuffer unsignedShortAt: sourceIndex + 6).
		decodedBuffer at: destIndex + 3 put: ((f*255 + 0.5) min: 255 max: 0) asInteger.

		sourceIndex := sourceIndex + 8.
		destIndex := destIndex + 4
	].
	
	Bitmap adoptInstance: decodedBuffer.
	^ Form extent: faceSize@faceSize depth: 32 bits: decodedBuffer.
]

{ #category : #accessing }
AGPULightProbeTextures >> specularLightProbe [

	^ specularLightProbe
]

{ #category : #accessing }
AGPULightProbeTextures >> specularLightProbe: anObject [

	specularLightProbe := anObject
]

{ #category : #accessing }
AGPULightProbeTextures >> specularLightProbeSize [

	^ specularLightProbeSize
]

{ #category : #accessing }
AGPULightProbeTextures >> specularLightProbeSize: anObject [

	specularLightProbeSize := anObject
]
