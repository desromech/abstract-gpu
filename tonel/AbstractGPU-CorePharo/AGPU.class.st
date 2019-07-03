Class {
	#name : #AGPU,
	#superclass : #AGPUBindingsCommon,
	#instVars : [
		'session',
		'mainPlatform',
		'mainDevice'
	],
	#classVars : [
		'UniqueInstance'
	],
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #'platform querying' }
AGPU class >> firstPlatform [
	^ self uniqueInstance firstPlatform
]

{ #category : #'main device' }
AGPU class >> mainDevice [
	^ self uniqueInstance mainDevice
]

{ #category : #'main device' }
AGPU class >> mainPlatform [
	^ self uniqueInstance mainPlatform
]

{ #category : #utilties }
AGPU class >> packListOfReferences: aListOfReferences [
	| pointerSize packedList destIndex |
	pointerSize := Smalltalk wordSize.
	packedList := ByteArray new: pointerSize * aListOfReferences size.
	destIndex := 1.
	aListOfReferences do: [ :ref |
		packedList pointerAt: destIndex put: ref validHandle getHandle.
		destIndex := destIndex + pointerSize
	].

	^ packedList
]

{ #category : #'platform querying' }
AGPU class >> platforms [
	^ self uniqueInstance platforms
]

{ #category : #accessing }
AGPU class >> session [
	^ Smalltalk session
]

{ #category : #'platform querying' }
AGPU class >> uniqueInstance [
	UniqueInstance ifNil: [ UniqueInstance := self new. ].
	^ UniqueInstance
]

{ #category : #'main device' }
AGPU >> checkSession [
	(session ~~ Smalltalk session or: [mainDevice isNil]) ifTrue: [
		self openMainDevice.
		session := Smalltalk session.
	].

]

{ #category : #'main device' }
AGPU >> debugLayerEnabled [
	^ true
]

{ #category : #'platform querying' }
AGPU >> firstPlatform [
	^ self platforms first
]

{ #category : #'main device' }
AGPU >> mainDevice [
	self checkSession.
	^ mainDevice
]

{ #category : #'main device' }
AGPU >> mainPlatform [
	self checkSession.
	^ mainPlatform
]

{ #category : #'main device' }
AGPU >> openMainDevice [
	| deviceOpenInfo |
	mainPlatform := self firstPlatform.
	mainPlatform ifNil: [ self error: 'No AGPU platform is available.' ].

	deviceOpenInfo := AGPUDeviceOpenInfo new debug_layer: self debugLayerEnabled asBit.
	mainDevice := mainPlatform openDevice: deviceOpenInfo.
	mainDevice ifNil: [ self error: 'Failed to open the main AGPU device' ].

]

{ #category : #'platform querying' }
AGPU >> platformCount [
	| platformCount |
	platformCount := ExternalAddress allocate: 4.
	[
		self getPlatforms: 0 platforms: nil ret_numplatforms: platformCount.
		^ platformCount unsignedLongAt: 1
	] ensure: [ platformCount free ].
]

{ #category : #'platform querying' }
AGPU >> platforms [
	| platformCount platformPointers platformCountRet pointerSize |
	platformCount := self platformCount.
	platformCountRet := ExternalAddress allocate: 4.
	[
		pointerSize := Smalltalk wordSize.
		platformPointers := ExternalAddress allocate: pointerSize * platformCount.
		[
			self getPlatforms: platformCount platforms: platformPointers ret_numplatforms: platformCountRet.
			platformCount := platformCountRet unsignedLongAt: 1.
			^ (1 to: platformCount) collect: [ :i | (Smalltalk at: #AGPUPlatform) forManualHandle: (platformPointers pointerAt: (i-1)*pointerSize + 1)]
		] ensure: [ platformPointers free ]
	] ensure: [ platformCountRet free ].
]
