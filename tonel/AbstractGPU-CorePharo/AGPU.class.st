"
I hold the global AGPU entry points, and some facilities for selecting devices and platforms.
"
Class {
	#name : #AGPU,
	#superclass : #AGPUBindingsCommon,
	#instVars : [
		'mainPlatform',
		'mainDevice',
		'deviceSession',
		'platformSessions',
		'platforms',
		'deviceDescriptions'
	],
	#classVars : [
		'AllowVR',
		'DebugLayerEnabled',
		'PreferredMainDeviceName',
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
			(aBuilder pickOne: #preferredMainDeviceName)
				domainValues: #('') , self availableMainDeviceNames;
				label: 'Preferred Main Device Name';
				description: 'This selects the name of the preffered main device.'.
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
AGPU class >> availableMainDeviceNames [
	^ self deviceDescriptions collect: [ :each | each fullName ]
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
AGPU class >> deviceDescriptions [
	^ self uniqueInstance deviceDescriptions
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

{ #category : #preferences }
AGPU class >> preferredMainDeviceName [
	^ PreferredMainDeviceName ifNil: [ PreferredMainDeviceName := '' ]
]

{ #category : #preferences }
AGPU class >> preferredMainDeviceName: aString [
	^ PreferredMainDeviceName := aString
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
AGPU >> checkDeviceSession [
	(deviceSession ~~ Smalltalk session or: [mainDevice isNil]) ifTrue: [
		self openMainDevice.
		deviceSession := Smalltalk session.
	].

]

{ #category : #'platform querying' }
AGPU >> checkPlatformsSession [
	self fetchPlatforms.
	self fetchDevices.
]

{ #category : #'main device' }
AGPU >> debugLayerEnabled [
	^ self class debugLayerEnabled
]

{ #category : #'platform querying' }
AGPU >> deviceDescriptions [
	self checkPlatformsSession.
	^ deviceDescriptions
]

{ #category : #private }
AGPU >> fetchDevices [
	deviceDescriptions := OrderedCollection new.
	platforms doWithIndex: [ :platform :platormIndex |
		| platformName |
		platformName := platform getName.
		0 to: platform getGpuCount - 1 do: [ :gpuIndex |
			deviceDescriptions add: (AGPUDeviceDescription new
				id: deviceDescriptions size;
				platformIndex: platormIndex - 1;
				gpuIndex: gpuIndex;
				platformName: platformName;
				name: (platform getGpuName: gpuIndex);
				type: (platform getGpuDeviceType: gpuIndex);
				yourself)
		]
	].
]

{ #category : #'platform querying' }
AGPU >> fetchPlatforms [
	| platformCount platformPointers platformCountRet pointerSize |
	platformCount := self platformCount.
	platformCountRet := ExternalAddress allocate: 4.
	platforms := [
		pointerSize := Smalltalk wordSize.
		platformPointers := ExternalAddress allocate: pointerSize * platformCount.
		[
			self getPlatforms: platformCount platforms: platformPointers ret_numplatforms: platformCountRet.
			platformCount := platformCountRet unsignedLongAt: 1.
			(1 to: platformCount) collect: [ :i | (Smalltalk at: #AGPUPlatform) forManualHandle: (platformPointers pointerAt: (i-1)*pointerSize + 1)]
		] ensure: [ platformPointers free ]
	] ensure: [ platformCountRet free ].
]

{ #category : #'platform querying' }
AGPU >> firstPlatform [
	^ self platforms first
]

{ #category : #'main device' }
AGPU >> mainDevice [
	self checkDeviceSession.
	^ mainDevice
]

{ #category : #'main device' }
AGPU >> mainPlatform [
	self checkDeviceSession.
	^ mainPlatform
]

{ #category : #'main device' }
AGPU >> openMainDevice [
	| deviceOpenInfo mainPlatformIndex mainGpuIndex |
	mainPlatformIndex := 0.
	mainGpuIndex := 0.
	self preferredMainDeviceName ifNotEmpty: [ 
		self deviceDescriptions detect: [ :each | each fullName = self preferredMainDeviceName ] ifFound: [ :mainDeviceDescription |
			mainPlatformIndex := mainDeviceDescription platformIndex.
			mainGpuIndex := mainDeviceDescription gpuIndex.
		]
	].

	self platforms ifEmpty: [ 
		self error: 'No AGPU platform is available.'
	].

	mainPlatform := platforms at: mainPlatformIndex + 1.
	mainPlatform ifNil: [ self error: 'No AGPU platform is available.' ].

	deviceOpenInfo := AGPUDeviceOpenInfo new
		gpu_index: mainGpuIndex;
		debug_layer: self debugLayerEnabled asBit.
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
	self checkPlatformsSession.
	^ platforms
]

{ #category : #preferences }
AGPU >> preferredMainDeviceName [
	^ self class preferredMainDeviceName
]
