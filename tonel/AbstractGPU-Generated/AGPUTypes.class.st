Class {
	#name : #AGPUTypes,
	#classVars : [
		'agpu_cstring',
		'agpu_texture_aspect',
		'agpu_string',
		'agpu_swap_chain',
		'agpu_texture_view_description',
		'agpu_field_type',
		'agpu_inheritance_info',
		'agpu_buffer_usage_type',
		'agpu_mapping_access',
		'agpu_size3d',
		'agpu_texture_address_mode',
		'agpu_float',
		'agpu_buffer_description',
		'agpu_shader_signature',
		'agpu_shader',
		'agpu_compare_function',
		'agpu_color4f',
		'agpu_size',
		'agpu_double',
		'agpu_filter',
		'agpu_blending_factor',
		'agpu_vertex_attrib_description',
		'agpu_framebuffer',
		'agpu_swap_chain_create_info',
		'agpu_sbyte',
		'agpu_shader_language',
		'agpu_device_open_info',
		'agpu_renderpass_color_attachment_description',
		'agpu_renderpass_attachment_action',
		'agpu_renderpass_depth_stencil_description',
		'agpu_command_list',
		'agpu_polygon_mode',
		'agpu_cstring_buffer',
		'agpu_vertex_binding',
		'agpu_shader_binding_type',
		'agpu_bool',
		'agpu_primitive_topology',
		'agpu_string_buffer',
		'agpu_platform',
		'agpu_shader_signature_builder',
		'agpu_texture_format',
		'agpu_command_list_type',
		'agpu_bitfield',
		'agpu_buffer',
		'agpu_region3d',
		'agpu_texture',
		'agpu_pointer',
		'agpu_int',
		'agpu_string_length',
		'agpu_vertex_layout',
		'agpu_subresource_range',
		'agpu_sampler_description',
		'agpu_buffer_mapping_flags',
		'agpu_device',
		'agpu_renderpass_description',
		'agpu_texture_flags',
		'agpu_stencil_operation',
		'agpu_draw_elements_command',
		'agpu_command_queue',
		'agpu_feature',
		'agpu_face_winding',
		'agpu_shader_resource_binding',
		'agpu_component_swizzle',
		'agpu_enum',
		'agpu_command_allocator',
		'agpu_texture_description',
		'agpu_byte',
		'agpu_cull_mode',
		'agpu_command_queue_type',
		'agpu_shader_type',
		'agpu_pipeline_state',
		'agpu_error',
		'agpu_uint',
		'agpu_fence',
		'agpu_short',
		'agpu_blending_operation',
		'agpu_buffer_binding_type',
		'agpu_components_swizzle',
		'agpu_compute_pipeline_builder',
		'agpu_render_buffer_bit',
		'agpu_depth_stencil_value',
		'agpu_texture_type',
		'agpu_pipeline_builder',
		'agpu_ushort',
		'agpu_renderpass'
	],
	#superclass : #SharedPool,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'initialize' }
AGPUTypes class >> initialize [
"
	self initialize
"
	super initialize.

	agpu_cstring := #'char*'.
	agpu_texture_aspect := #int.
	agpu_string := #'char*'.
	agpu_swap_chain := #'void'.
	agpu_texture_view_description := AGPUTextureViewDescription.
	agpu_field_type := #int.
	agpu_inheritance_info := AGPUInheritanceInfo.
	agpu_buffer_usage_type := #int.
	agpu_mapping_access := #int.
	agpu_size3d := AGPUSize3d.
	agpu_texture_address_mode := #int.
	agpu_float := #'float'.
	agpu_buffer_description := AGPUBufferDescription.
	agpu_shader_signature := #'void'.
	agpu_shader := #'void'.
	agpu_compare_function := #int.
	agpu_color4f := AGPUColor4f.
	agpu_size := #'uint'.
	agpu_double := #'double'.
	agpu_filter := #int.
	agpu_blending_factor := #int.
	agpu_vertex_attrib_description := AGPUVertexAttribDescription.
	agpu_framebuffer := #'void'.
	agpu_swap_chain_create_info := AGPUSwapChainCreateInfo.
	agpu_sbyte := #'char'.
	agpu_shader_language := #int.
	agpu_device_open_info := AGPUDeviceOpenInfo.
	agpu_renderpass_color_attachment_description := AGPURenderpassColorAttachmentDescription.
	agpu_renderpass_attachment_action := #int.
	agpu_renderpass_depth_stencil_description := AGPURenderpassDepthStencilDescription.
	agpu_command_list := #'void'.
	agpu_polygon_mode := #int.
	agpu_cstring_buffer := #'char*'.
	agpu_vertex_binding := #'void'.
	agpu_shader_binding_type := #int.
	agpu_bool := #'int'.
	agpu_primitive_topology := #int.
	agpu_string_buffer := #'char*'.
	agpu_platform := #'void'.
	agpu_shader_signature_builder := #'void'.
	agpu_texture_format := #int.
	agpu_command_list_type := #int.
	agpu_bitfield := #'uint'.
	agpu_buffer := #'void'.
	agpu_region3d := AGPURegion3d.
	agpu_texture := #'void'.
	agpu_pointer := #'void*'.
	agpu_int := #'int'.
	agpu_string_length := #'int'.
	agpu_vertex_layout := #'void'.
	agpu_subresource_range := AGPUSubresourceRange.
	agpu_sampler_description := AGPUSamplerDescription.
	agpu_buffer_mapping_flags := #int.
	agpu_device := #'void'.
	agpu_renderpass_description := AGPURenderpassDescription.
	agpu_texture_flags := #int.
	agpu_stencil_operation := #int.
	agpu_draw_elements_command := AGPUDrawElementsCommand.
	agpu_command_queue := #'void'.
	agpu_feature := #int.
	agpu_face_winding := #int.
	agpu_shader_resource_binding := #'void'.
	agpu_component_swizzle := #int.
	agpu_enum := #'int'.
	agpu_command_allocator := #'void'.
	agpu_texture_description := AGPUTextureDescription.
	agpu_byte := #'uchar'.
	agpu_cull_mode := #int.
	agpu_command_queue_type := #int.
	agpu_shader_type := #int.
	agpu_pipeline_state := #'void'.
	agpu_error := #int.
	agpu_uint := #'uint'.
	agpu_fence := #'void'.
	agpu_short := #'short'.
	agpu_blending_operation := #int.
	agpu_buffer_binding_type := #int.
	agpu_components_swizzle := AGPUComponentsSwizzle.
	agpu_compute_pipeline_builder := #'void'.
	agpu_render_buffer_bit := #int.
	agpu_depth_stencil_value := AGPUDepthStencilValue.
	agpu_texture_type := #int.
	agpu_pipeline_builder := #'void'.
	agpu_ushort := #'ushort'.
	agpu_renderpass := #'void'.
]

