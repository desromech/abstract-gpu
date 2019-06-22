Class {
	#name : #AGPUSamplerDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUSamplerDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_filter filter;
		 agpu_texture_address_mode address_u;
		 agpu_texture_address_mode address_v;
		 agpu_texture_address_mode address_w;
		 agpu_float mip_lod_bias;
		 agpu_float maxanisotropy;
		 agpu_bool comparison_enabled;
		 agpu_compare_function comparison_function;
		 agpu_color4f border_color;
		 agpu_float min_lod;
		 agpu_float max_lod;
	)
]

