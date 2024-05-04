Class {
	#name : #AGPUVrRenderModel,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrRenderModel class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint vertex_count;
		 agpu_vr_render_model_vertex* vertices;
		 agpu_uint triangle_count;
		 agpu_ushort* indices;
		 agpu_vr_render_model_texture* texture;
	)
]

