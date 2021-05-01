Class {
	#name : #AGPUSampleImmediateRendererCommon,
	#superclass : #AGPUMorph,
	#instVars : [
		'mainRenderPass'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> checkboardNormalMapTextureForm [
	^ self generateProceduralNormalMapWithExtent: 64@64 withHeightFunction: [:x :y | self checkboardWithCellSize: 16 valueAtX: x y: y]
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> checkboardTextureForm [
	^ self generateProceduralFormWithExtent: 64@64 withGrayFunction: [:x :y | self checkboardWithCellSize: 16 valueAtX: x y: y]
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> checkboardWithCellSize: cellSize valueAtX: x y: y [
	| cellY cellX |
	cellY := 	(y // cellSize) odd.
	cellX := 	(x // cellSize) odd.
	^ (cellX xor: cellY) asBit
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> generateProceduralFormWithExtent: extent withColorFunction: colorFunction [
	| form destIndex bits |
	form := Form extent: extent depth: 32.
	destIndex := 1.
	bits := form bits.
	0 to: 63 do: [ :y |
		0 to: 63 do: [ :x |
			| texelColor encodedColor b g r a|
			texelColor := colorFunction value: x value: y.
			r := (texelColor red*16rff) floor min: 16rff max: 0.
			g := (texelColor green*16rff) floor min: 16rff max: 0.
			b := (texelColor blue*16rff) floor min: 16rff max: 0.
			a := 16rff.
			
			encodedColor := ((b bitOr: (g bitShift: 8)) bitOr: (r bitShift: 16)) bitOr: (a bitShift: 24).
			bits at: destIndex put: encodedColor.
			destIndex := destIndex + 1
		].
	].
	^ form
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> generateProceduralFormWithExtent: extent withGrayFunction: grayFunction [
	^ self generateProceduralFormWithExtent: extent withColorFunction: [:x :y |
		| gray |
		gray := grayFunction value: x value: y.
		Color r: gray g: gray b: gray alpha: 1.0
	]
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> generateProceduralNormalMapWithExtent: extent withHeightFunction: heightFunction [
	^ self generateProceduralFormWithExtent: extent withColorFunction: [:x :y |
		| dx dy nx ny nz nl |
		dx := (heightFunction value: x + 1 value: y) - (heightFunction value: x - 1 value: y).
		dy := (heightFunction value: x value: y + 1) - (heightFunction value: x value: y - 1).
		
		"
		tangent: (step, 0.0, dx)
		bitangent: (0.0, step, dy)
		normal: tangent x bitangent => (-step * dx, -step *dy, step * step)
		"
		nx := -2.0 * dx.
		ny := -2.0 * dy.
		nz := 4.0.
		nl := ((nx*nx) + (ny*ny) + (nz*nz)) sqrt.
		nx := nx / nl * 0.5 + 0.5.
		ny := ny / nl * 0.5 + 0.5.
		nz := nz / nl * 0.5 + 0.5.
		Color r: nx g: ny b: nz alpha: 1.0
	]
]

{ #category : #initialization }
AGPUSampleImmediateRendererCommon >> initializeForNewSession [
	super initializeForNewSession.
	
	mainRenderPass := self createRenderPassWithColor: Color black.
]
