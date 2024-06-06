Class {
	#name : #AGPUWindowScraperHandle,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_window_scraper_handle: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_window_scraper_handle: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> isValid [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isValid_window_scraper_handle: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> isVisible [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isVisible_window_scraper_handle: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> getWidth [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getWidth_window_scraper_handle: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> getHeight [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getHeight_window_scraper_handle: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraperHandle >> captureInTexture [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance captureInTexture_window_scraper_handle: (self validHandle).
	^ AGPUTexture forHandle: resultValue_
]

