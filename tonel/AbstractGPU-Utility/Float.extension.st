Extension { #name : #Float }

{ #category : #'*AbstractGPU-Utility' }
Float class >> fromIEEE16Bit: halfWord [
	"Convert the given 16 bit word (which is supposed to be a positive 16-bit value) from
	 a 16 bit IEEE floating point representation into an actual float object (being
	 64 bits wide)."
	
	| sign mantissa exponent delta |
	halfWord <= 0 ifTrue:
		[^halfWord negative
			ifTrue: [self error: 'Cannot deal with negative numbers']
			ifFalse: [self zero]].
	sign := halfWord bitAnd: 16r8000.
	halfWord = sign ifTrue:
		[^self negativeZero].
	
	exponent := ((halfWord bitShift: -10) bitAnd: 16r1F) - 15.
	mantissa := halfWord bitAnd: 16r3FF.

	exponent = 16 ifTrue: "Either NAN or INF"
		[^mantissa = 0
			ifTrue:
				[sign = 0 
					ifTrue: [self infinity]
					ifFalse: [self negativeInfinity]]
			ifFalse: [self nan]].

	exponent = -15 ifTrue:
		"gradual underflow (denormalized number)
		 Remove first bit of mantissa and adjust exponent"
		[delta := mantissa highBit.	
		 mantissa := (mantissa bitAnd: (1 bitShift: delta - 1) - 1) bitShift: 11 - delta.
		 exponent := exponent + delta - 10].
	
	"Create new float"
	^(self basicNew: 2)
		basicAt: 1 put: ((sign bitOr: (1023 + exponent bitShift: 20)) bitOr: (mantissa bitShift: 10));
		* 1.0 "reduce to SmallFloat64 if possible"
]
