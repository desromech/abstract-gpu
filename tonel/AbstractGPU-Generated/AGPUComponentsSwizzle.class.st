Class {
	#name : #AGPUComponentsSwizzle,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUComponentsSwizzle class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_component_swizzle r;
		 agpu_component_swizzle g;
		 agpu_component_swizzle b;
		 agpu_component_swizzle a;
		)
]

