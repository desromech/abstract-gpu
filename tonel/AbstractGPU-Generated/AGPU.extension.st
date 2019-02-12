Extension { #name : #AGPU }

{ #category : #'*AbstractGPU-Generated' }
AGPU >> getPlatforms: numplatforms platforms: platforms ret_numplatforms: ret_numplatforms [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getPlatforms_numplatforms: numplatforms platforms: platforms ret_numplatforms: ret_numplatforms.
	self checkErrorCode: resultValue_
]

