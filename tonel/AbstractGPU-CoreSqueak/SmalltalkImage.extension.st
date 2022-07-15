Extension { #name : #SmalltalkImage }

{ #category : #'*AbstractGPU-CoreSqueak' }
SmalltalkImage >> session [
	^ self at: #DoItFirst ifPresent: [:cls | cls current] ifAbsent: [AGPU session]
]
