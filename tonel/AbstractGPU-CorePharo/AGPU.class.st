Class {
	#name : #AGPU,
	#superclass : #AGPUBindingsCommon,
	#instVars : [
		'session',
		'mainPlatform',
		'mainDevice'
	],
	#classVars : [
		'AllowVR',
		'DebugLayerEnabled',
		'UniqueInstance',
		'VSyncEnabled'
	],
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #preferences }
AGPU class >> agpuSettingsOn: aBuilder [
	<systemsettings>
	(aBuilder group: #agpu)
		label: 'AbstractGPU';
		description: 'AbstractGPU Settings';
		with: [
			(aBuilder setting: #debugLayerEnabled)
				label: 'Debug Layer enabled';
				description: 'This enables the graphics API debug layer (Vulkan Only).'.
			(aBuilder setting: #vsyncEnabled)
				label: 'Enable VSync by default';
				description: 'This enables the use by default of vsync.'.
			(aBuilder setting: #allowVR)
				label: 'Allow VR';
				description: 'This enables the support for VR (OpenVR headsets such as the HTC Vive).'.
		]
]

{ #category : #preferences }
AGPU class >> allowVR [
	^ AllowVR ifNil: [ AllowVR := false ]
]

{ #category : #preferences }
AGPU class >> allowVR: aBoolean [
	AllowVR := aBoolean
]

{ #category : #preferences }
AGPU class >> debugLayerEnabled [
	^ DebugLayerEnabled ifNil: [ DebugLayerEnabled := false ]
]

{ #category : #preferences }
AGPU class >> debugLayerEnabled: aBoolean [
	^ DebugLayerEnabled := aBoolean
]

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

{ #category : #preferences }
AGPU class >> shouldUseVSync [
	^ self allowVR not and: [ self vsyncEnabled ]
]

{ #category : #'platform querying' }
AGPU class >> uniqueInstance [
	UniqueInstance ifNil: [ UniqueInstance := self new. ].
	^ UniqueInstance
]

{ #category : #preferences }
AGPU class >> vsyncEnabled [
	^ VSyncEnabled ifNil: [ VSyncEnabled := true ]
]

{ #category : #preferences }
AGPU class >> vsyncEnabled: aBoolean [
	VSyncEnabled := aBoolean
]

{ #category : #'main device' }
AGPU >> allowVR [
	^ self class allowVR
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
	^ self class debugLayerEnabled
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
	self allowVR ifTrue: [ 
		deviceOpenInfo open_flags: (AGPUConstants bindingOf: #AGPU_DEVICE_OPEN_FLAG_ALLOW_VR) value.
	].

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
