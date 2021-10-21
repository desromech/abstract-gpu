Class {
	#name : #AGPUTypes,
	#superclass : #SharedPool,
	#classVars : [
		'agpu_access_flags',
		'agpu_bitfield',
		'agpu_blending_factor',
		'agpu_blending_operation',
		'agpu_bool',
		'agpu_buffer',
		'agpu_buffer_description',
		'agpu_buffer_image_copy_region',
		'agpu_buffer_mapping_flags',
		'agpu_buffer_usage_mask',
		'agpu_byte',
		'agpu_color4f',
		'agpu_command_allocator',
		'agpu_command_list',
		'agpu_command_list_type',
		'agpu_command_queue',
		'agpu_command_queue_type',
		'agpu_compare_function',
		'agpu_component_swizzle',
		'agpu_components_swizzle',
		'agpu_compute_pipeline_builder',
		'agpu_cstring',
		'agpu_cstring_buffer',
		'agpu_cull_mode',
		'agpu_depth_stencil_value',
		'agpu_device',
		'agpu_device_open_flags',
		'agpu_device_open_info',
		'agpu_device_type',
		'agpu_double',
		'agpu_draw_elements_command',
		'agpu_enum',
		'agpu_error',
		'agpu_face_winding',
		'agpu_feature',
		'agpu_fence',
		'agpu_field_type',
		'agpu_filter',
		'agpu_float',
		'agpu_framebuffer',
		'agpu_frustum_tangents',
		'agpu_image_copy_region',
		'agpu_immediate_renderer',
		'agpu_immediate_renderer_fog_mode',
		'agpu_immediate_renderer_light',
		'agpu_immediate_renderer_light_classic',
		'agpu_immediate_renderer_light_pbr',
		'agpu_immediate_renderer_lighting_model',
		'agpu_immediate_renderer_material',
		'agpu_immediate_renderer_material_classic',
		'agpu_immediate_renderer_material_flat_color',
		'agpu_immediate_renderer_material_metallic_roughness',
		'agpu_immediate_renderer_texture_binding',
		'agpu_immediate_renderer_vertex_attribute',
		'agpu_inheritance_info',
		'agpu_int',
		'agpu_limit',
		'agpu_mapping_access',
		'agpu_matrix3x3f',
		'agpu_matrix4x4f',
		'agpu_memory_heap_type',
		'agpu_offline_shader_compiler',
		'agpu_offset3d',
		'agpu_pipeline_builder',
		'agpu_pipeline_stage_flags',
		'agpu_pipeline_state',
		'agpu_platform',
		'agpu_pointer',
		'agpu_polygon_mode',
		'agpu_primitive_topology',
		'agpu_quaternionf',
		'agpu_region3d',
		'agpu_render_buffer_bit',
		'agpu_renderpass',
		'agpu_renderpass_attachment_action',
		'agpu_renderpass_color_attachment_description',
		'agpu_renderpass_depth_stencil_description',
		'agpu_renderpass_description',
		'agpu_sampler',
		'agpu_sampler_description',
		'agpu_sbyte',
		'agpu_shader',
		'agpu_shader_binding_type',
		'agpu_shader_language',
		'agpu_shader_resource_binding',
		'agpu_shader_signature',
		'agpu_shader_signature_builder',
		'agpu_shader_type',
		'agpu_short',
		'agpu_size',
		'agpu_size2d',
		'agpu_size3d',
		'agpu_state_tracker',
		'agpu_state_tracker_cache',
		'agpu_stencil_operation',
		'agpu_string',
		'agpu_string_buffer',
		'agpu_string_length',
		'agpu_swap_chain',
		'agpu_swap_chain_create_info',
		'agpu_swap_chain_flags',
		'agpu_swap_chain_presentation_mode',
		'agpu_texture',
		'agpu_texture_address_mode',
		'agpu_texture_aspect',
		'agpu_texture_clear_value',
		'agpu_texture_description',
		'agpu_texture_format',
		'agpu_texture_subresource_level',
		'agpu_texture_subresource_range',
		'agpu_texture_type',
		'agpu_texture_usage_mode_mask',
		'agpu_texture_view',
		'agpu_texture_view_description',
		'agpu_uint',
		'agpu_ushort',
		'agpu_vector2f',
		'agpu_vector3f',
		'agpu_vector4f',
		'agpu_vertex_attrib_description',
		'agpu_vertex_binding',
		'agpu_vertex_layout',
		'agpu_vr_button',
		'agpu_vr_controller_event',
		'agpu_vr_dual_analog_event',
		'agpu_vr_dual_analog_which',
		'agpu_vr_event',
		'agpu_vr_event_data',
		'agpu_vr_event_type',
		'agpu_vr_eye',
		'agpu_vr_generic_event',
		'agpu_vr_system',
		'agpu_vr_tracked_device_class',
		'agpu_vr_tracked_device_pose',
		'agpu_vr_tracked_device_role'
	],
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #initialize }
AGPUTypes class >> initialize [
"
	self initialize
"
	super initialize.

	agpu_error := #int.
	agpu_device_open_flags := #int.
	agpu_swap_chain_flags := #int.
	agpu_swap_chain_presentation_mode := #int.
	agpu_command_queue_type := #int.
	agpu_access_flags := #int.
	agpu_pipeline_stage_flags := #int.
	agpu_primitive_topology := #int.
	agpu_polygon_mode := #int.
	agpu_device_type := #int.
	agpu_feature := #int.
	agpu_limit := #int.
	agpu_renderpass_attachment_action := #int.
	agpu_stencil_operation := #int.
	agpu_compare_function := #int.
	agpu_texture_type := #int.
	agpu_texture_usage_mode_mask := #int.
	agpu_shader_type := #int.
	agpu_memory_heap_type := #int.
	agpu_buffer_usage_mask := #int.
	agpu_buffer_mapping_flags := #int.
	agpu_mapping_access := #int.
	agpu_render_buffer_bit := #int.
	agpu_shader_binding_type := #int.
	agpu_shader_language := #int.
	agpu_field_type := #int.
	agpu_texture_format := #int.
	agpu_filter := #int.
	agpu_texture_address_mode := #int.
	agpu_texture_aspect := #int.
	agpu_command_list_type := #int.
	agpu_blending_factor := #int.
	agpu_blending_operation := #int.
	agpu_component_swizzle := #int.
	agpu_face_winding := #int.
	agpu_cull_mode := #int.
	agpu_vr_eye := #int.
	agpu_vr_tracked_device_class := #int.
	agpu_vr_tracked_device_role := #int.
	agpu_vr_button := #int.
	agpu_vr_dual_analog_which := #int.
	agpu_vr_event_type := #int.
	agpu_immediate_renderer_vertex_attribute := #int.
	agpu_immediate_renderer_lighting_model := #int.
	agpu_immediate_renderer_fog_mode := #int.
	agpu_immediate_renderer_texture_binding := #int.
	agpu_byte := #'uchar'.
	agpu_sbyte := #'char'.
	agpu_short := #'short'.
	agpu_ushort := #'ushort'.
	agpu_int := #'int'.
	agpu_uint := #'uint'.
	agpu_pointer := #'void*'.
	agpu_size := #'uint'.
	agpu_enum := #'int'.
	agpu_bool := #'int'.
	agpu_float := #'float'.
	agpu_double := #'double'.
	agpu_bitfield := #'uint'.
	agpu_cstring := #'char*'.
	agpu_string := #'byte*'.
	agpu_string_length := #'int'.
	agpu_cstring_buffer := #'char*'.
	agpu_string_buffer := #'byte*'.
	agpu_platform := #'void'.
	agpu_device := #'void'.
	agpu_vr_system := #'void'.
	agpu_swap_chain := #'void'.
	agpu_compute_pipeline_builder := #'void'.
	agpu_pipeline_builder := #'void'.
	agpu_pipeline_state := #'void'.
	agpu_command_queue := #'void'.
	agpu_command_allocator := #'void'.
	agpu_command_list := #'void'.
	agpu_texture := #'void'.
	agpu_texture_view := #'void'.
	agpu_sampler := #'void'.
	agpu_buffer := #'void'.
	agpu_vertex_binding := #'void'.
	agpu_vertex_layout := #'void'.
	agpu_shader := #'void'.
	agpu_framebuffer := #'void'.
	agpu_renderpass := #'void'.
	agpu_shader_signature_builder := #'void'.
	agpu_shader_signature := #'void'.
	agpu_shader_resource_binding := #'void'.
	agpu_fence := #'void'.
	agpu_offline_shader_compiler := #'void'.
	agpu_state_tracker_cache := #'void'.
	agpu_state_tracker := #'void'.
	agpu_immediate_renderer := #'void'.
	agpu_device_open_info := AGPUDeviceOpenInfo.
	agpu_swap_chain_create_info := AGPUSwapChainCreateInfo.
	agpu_buffer_description := AGPUBufferDescription.
	agpu_color4f := AGPUColor4f.
	agpu_depth_stencil_value := AGPUDepthStencilValue.
	agpu_texture_clear_value := AGPUTextureClearValue.
	agpu_texture_description := AGPUTextureDescription.
	agpu_components_swizzle := AGPUComponentsSwizzle.
	agpu_texture_subresource_range := AGPUTextureSubresourceRange.
	agpu_texture_subresource_level := AGPUTextureSubresourceLevel.
	agpu_texture_view_description := AGPUTextureViewDescription.
	agpu_draw_elements_command := AGPUDrawElementsCommand.
	agpu_vertex_attrib_description := AGPUVertexAttribDescription.
	agpu_sampler_description := AGPUSamplerDescription.
	agpu_renderpass_color_attachment_description := AGPURenderpassColorAttachmentDescription.
	agpu_renderpass_depth_stencil_description := AGPURenderpassDepthStencilDescription.
	agpu_renderpass_description := AGPURenderpassDescription.
	agpu_inheritance_info := AGPUInheritanceInfo.
	agpu_vector2f := AGPUVector2f.
	agpu_vector3f := AGPUVector3f.
	agpu_vector4f := AGPUVector4f.
	agpu_quaternionf := AGPUQuaternionf.
	agpu_matrix3x3f := AGPUMatrix3x3f.
	agpu_matrix4x4f := AGPUMatrix4x4f.
	agpu_size2d := AGPUSize2d.
	agpu_size3d := AGPUSize3d.
	agpu_frustum_tangents := AGPUFrustumTangents.
	agpu_offset3d := AGPUOffset3d.
	agpu_region3d := AGPURegion3d.
	agpu_buffer_image_copy_region := AGPUBufferImageCopyRegion.
	agpu_image_copy_region := AGPUImageCopyRegion.
	agpu_vr_tracked_device_pose := AGPUVrTrackedDevicePose.
	agpu_vr_generic_event := AGPUVrGenericEvent.
	agpu_vr_controller_event := AGPUVrControllerEvent.
	agpu_vr_dual_analog_event := AGPUVrDualAnalogEvent.
	agpu_vr_event_data := AGPUVrEventData.
	agpu_vr_event := AGPUVrEvent.
	agpu_immediate_renderer_light_classic := AGPUImmediateRendererLightClassic.
	agpu_immediate_renderer_light_pbr := AGPUImmediateRendererLightPbr.
	agpu_immediate_renderer_light := AGPUImmediateRendererLight.
	agpu_immediate_renderer_material_classic := AGPUImmediateRendererMaterialClassic.
	agpu_immediate_renderer_material_metallic_roughness := AGPUImmediateRendererMaterialMetallicRoughness.
	agpu_immediate_renderer_material_flat_color := AGPUImmediateRendererMaterialFlatColor.
	agpu_immediate_renderer_material := AGPUImmediateRendererMaterial.
]
