Class {
	#name : #AGPUSampleImmediateRendererCommon,
	#superclass : #AGPUMorph,
	#instVars : [
		'mainRenderPass'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> checkboardTextureForm [
	| form |
	form := Form extent: 32@32 depth: 32.
	form getCanvas
		fillRectangle: (0@0 extent: 32@32) color: Color white;
		fillRectangle: (0@0 extent: 16@16) color: Color black;
		fillRectangle: (16@16 extent: 16@16) color: Color black.
	^ form
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> initializeForNewSession [
	super initializeForNewSession.
	
	mainRenderPass := self createRenderPassWithColor: Color black.
]
