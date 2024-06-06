Class {
	#name : #AGPUWindowScraper,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUWindowScraper >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_window_scraper: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraper >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_window_scraper: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraper >> enumerateWindows [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance enumerateWindows_window_scraper: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraper >> getWindowTitle: index [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getWindowTitle_window_scraper: (self validHandle) index: index.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUWindowScraper >> createWindowHandle: index [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createWindowHandle_window_scraper: (self validHandle) index: index.
	^ AGPUWindowScraperHandle forHandle: resultValue_
]

