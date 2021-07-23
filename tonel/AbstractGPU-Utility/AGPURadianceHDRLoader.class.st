"
I am an utility for loading Radiance HDR formats which is commonly used for light probes and sky environment maps.
"
Class {
	#name : #AGPURadianceHDRLoader,
	#superclass : #Object,
	#instVars : [
		'properties',
		'format',
		'width',
		'pitch',
		'height',
		'pixels'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #parsing }
AGPURadianceHDRLoader class >> parseFrom: inputStream [
	^ self new parseFrom: inputStream; yourself
]

{ #category : #accessing }
AGPURadianceHDRLoader >> asForm [
	^ self asFormWithExposure: 1.0
]

{ #category : #accessing }
AGPURadianceHDRLoader >> asFormWithExposure: exposure [
	^ self asFormWithExposure: exposure gamma: 2.2
]

{ #category : #accessing }
AGPURadianceHDRLoader >> asFormWithExposure: exposure gamma: gamma [
	^ Form extent: self extent depth: 32 bits: (self decodeWithExposure: exposure gamma: gamma reciprocal)
]

{ #category : #accessing }
AGPURadianceHDRLoader >> asFormWithRawData [
	| bits |
	bits := pixels shallowCopy.
	Bitmap adoptInstance: bits.
	^ Form extent: self extent depth: 32 bits: bits
]

{ #category : #accessing }
AGPURadianceHDRLoader >> decodeWithExposure: exposure gamma: gamma [
	| destPixels destRowIndex sourceRowIndex destPitch |
	destPitch := width*4.
	destPixels := ByteArray new: destPitch*height.
	destRowIndex := 1.
	sourceRowIndex := 1.
	
	1 to: height do: [ :y |
		| destIndex sourceIndex |
		destIndex := destRowIndex.
		sourceIndex := sourceRowIndex.
		1 to: width do: [ :x |
			|sr sg sb se fr fg fb dr dg db |
			sr := pixels at: sourceIndex.
			sg := pixels at: sourceIndex + 1.
			sb := pixels at: sourceIndex + 2.
			se := pixels at: sourceIndex + 3.
			
			se > 0 ifTrue: [ 
				| exp |
				exp := se - 136.
				fr := sr + 0.5 timesTwoPower: exp.
				fg := sg + 0.5 timesTwoPower: exp.
				fb := sb + 0.5 timesTwoPower: exp.
			] ifFalse: [
				fr := 0.0.
				fg := 0.0.
				fb := 0.0.
			].
			
			dr := ((fr * exposure raisedTo: gamma) *255.0 + 0.5) asInteger min: 255 max: 0.
			dg := ((fg * exposure raisedTo: gamma) *255.0 + 0.5) asInteger min: 255 max: 0.
			db := ((fb * exposure raisedTo: gamma) *255.0 + 0.5) asInteger min: 255 max: 0.
			
			destPixels at: destIndex put: db.
			destPixels at: destIndex + 1 put: dg.
			destPixels at: destIndex + 2 put: dr.
			destPixels at: destIndex + 3 put: 255.

			destIndex := destIndex + 4.
			sourceIndex := sourceIndex + 4.
		].
	
		sourceRowIndex := sourceRowIndex + pitch.
		destRowIndex := destRowIndex + destPitch
	].

	Bitmap adoptInstance: destPixels.
	^ destPixels
]

{ #category : #accessing }
AGPURadianceHDRLoader >> extent [
	^ width @ height
]

{ #category : #accessing }
AGPURadianceHDRLoader >> format [
	^ format
]

{ #category : #accessing }
AGPURadianceHDRLoader >> height [
	^ height
]

{ #category : #parsing }
AGPURadianceHDRLoader >> nextLineIn: inputStream [
	^ ByteString streamContents: [ :out |
		|c|
		[
			c := inputStream next.
			c ~~ nil and: [c ~= "lf" 10] ] whileTrue: [ 
			out nextPut: c asCharacter
		]
	]
]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseFrom: inputStream [
	self
		parseHeader: inputStream;
		parseScanlines: inputStream
	
]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseHeader: inputStream [
	| line |
	(inputStream next: 10) = '#?RADIANCE' asByteArray ifFalse: [
		self error: 'Invalid radiance header'
	].
	self skipLineIn: inputStream.
	
	properties := Dictionary new.
	[
		line := self nextLineIn: inputStream.
		line isNotEmpty 
	] whileTrue: [
		self parseInformationLine: line.
	].

	format := properties at: 'FORMAT' ifAbsent: [ self error: 'FORMAT property is not present.' ].
	(format = '32-bit_rle_rgbe' or: [ format = '32-bit_rle_xyze' ]) ifFalse: [ 
		self error: 'Unsupported format in hdr image: ' , format
	].

	self parseResolutionLine: (self nextLineIn: inputStream)
]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseInformationLine: line [
	| lineWithoutComment index key value |
	lineWithoutComment := line.
	index := line indexOf: $#.
	index > 0 ifTrue: [ lineWithoutComment := line first: index - 1].
	
	lineWithoutComment isEmpty ifTrue: [ ^ self ].

	index := lineWithoutComment indexOf: $=.
	index <= 0 ifTrue: [ ^ self ].

	key := line first: index - 1.
	value := line allButFirst: index.
	properties at: key put: value
]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseResolutionLine: line [
	| components |
	components := line trimBoth splitOn: ' '.
	(components first = '-Y' and: [ components third = '+X' ]) ifFalse: [ 
		self error: 'Unsupported hdr with non standard coordinate system: ' , line
	].

	width := components fourth asInteger.
	height := components second asInteger
]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseScaline: inputStream length: scanlineLength runLengthInto: scanlineDestIndex [
	| remainingLength destIndex |
	remainingLength := scanlineLength.
	destIndex := scanlineDestIndex.
	[remainingLength > 0] whileTrue: [ 
		| runCount |
		runCount := inputStream next.
		runCount > 128 ifTrue: [
			"Run length encoding."
			| runByte |
			runCount := runCount bitAnd: 127.
			runCount > remainingLength ifTrue: [ self error: 'Overrun' ].

			runByte := inputStream next.
			1 to: runCount do: [ :i |
				pixels at: destIndex put: runByte.
				destIndex := destIndex + 4.
				remainingLength :=	 remainingLength - 1.
			]
		] ifFalse: [
			"Pixel data"
			runCount > remainingLength ifTrue: [ self error: 'Overrun' ].
			1 to: runCount do: [ :b |
				pixels at: destIndex put: inputStream next.
				destIndex := destIndex + 4.
				remainingLength := remainingLength - 1.
			].
		].
	].

]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseScanline: inputStream into: destIndex [
	| scanlineStart scanlineLength |
	scanlineStart := inputStream next: 4.
	self assert: scanlineStart first = 2.
	self assert: scanlineStart second = 2.
	scanlineLength := (scanlineStart third * 256) + scanlineStart fourth.
	scanlineLength = width ifFalse: [ self error: 'Length mismatch' ].
	
	self
		parseScaline: inputStream length: scanlineLength runLengthInto: destIndex;
		parseScaline: inputStream length: scanlineLength runLengthInto: destIndex + 1;
		parseScaline: inputStream length: scanlineLength runLengthInto: destIndex + 2;
		parseScaline: inputStream length: scanlineLength runLengthInto: destIndex + 3.
]

{ #category : #parsing }
AGPURadianceHDRLoader >> parseScanlines: inputStream [
	| destIndex |
	pitch := width * 4.
	pixels := ByteArray new: pitch * height * 4.
	destIndex := 1.
	1 to: height do: [ :y |
		self parseScanline: inputStream into: destIndex.
		destIndex := destIndex + pitch
	]

]

{ #category : #accessing }
AGPURadianceHDRLoader >> pitch [
	^ pitch
]

{ #category : #accessing }
AGPURadianceHDRLoader >> properties [
	^ properties
]

{ #category : #parsing }
AGPURadianceHDRLoader >> skipLineIn: inputStream [

	[
		| c |
		c := inputStream next.
		c ~~ nil and: [c ~= "lf" 10]
	] whileTrue.

]

{ #category : #accessing }
AGPURadianceHDRLoader >> width [
	^ width
]
