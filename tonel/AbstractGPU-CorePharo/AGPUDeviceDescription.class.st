"
I hold a description for an AGPU device.
"
Class {
	#name : #AGPUDeviceDescription,
	#superclass : #Object,
	#instVars : [
		'id',
		'platformIndex',
		'gpuIndex',
		'platformName',
		'name',
		'type',
		'fullName'
	],
	#category : #'AbstractGPU-CorePharo'
}

{ #category : #accessing }
AGPUDeviceDescription >> fullName [
	^ fullName ifNil: [fullName := platformName  , ': ' , name]
]

{ #category : #accessing }
AGPUDeviceDescription >> gpuIndex [

	^ gpuIndex
]

{ #category : #accessing }
AGPUDeviceDescription >> gpuIndex: anObject [

	gpuIndex := anObject
]

{ #category : #accessing }
AGPUDeviceDescription >> id [

	^ id
]

{ #category : #accessing }
AGPUDeviceDescription >> id: anObject [

	id := anObject
]

{ #category : #accessing }
AGPUDeviceDescription >> name [

	^ name
]

{ #category : #accessing }
AGPUDeviceDescription >> name: anObject [

	name := anObject
]

{ #category : #accessing }
AGPUDeviceDescription >> platformIndex [

	^ platformIndex
]

{ #category : #accessing }
AGPUDeviceDescription >> platformIndex: anObject [

	platformIndex := anObject
]

{ #category : #accessing }
AGPUDeviceDescription >> platformName [

	^ platformName
]

{ #category : #accessing }
AGPUDeviceDescription >> platformName: anObject [

	platformName := anObject
]

{ #category : #accessing }
AGPUDeviceDescription >> printOn: aStream [
	aStream nextPutAll: self fullName
]

{ #category : #accessing }
AGPUDeviceDescription >> type [

	^ type
]

{ #category : #accessing }
AGPUDeviceDescription >> type: anObject [

	type := anObject
]
