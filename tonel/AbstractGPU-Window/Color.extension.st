Extension { #name : #Color }

{ #category : #'*AbstractGPU-Window' }
Color >> asAGPUColor4f [
	^ AGPUColor4f new
		r: self red;
		g: self green;
		b: self blue;
		a: self alpha;
		yourself
]
