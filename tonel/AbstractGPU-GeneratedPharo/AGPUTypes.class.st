Class {
	#name : #AGPUTypes,
	#classVars : [
		'agpu_matrix3x3f',
		'agpu_immediate_renderer_fog_mode',
		'agpu_field_type',
		'agpu_buffer_description',
		'agpu_matrix4x4f',
		'agpu_filter',
		'agpu_vertex_attrib_description',
		'agpu_framebuffer',
		'agpu_swap_chain_create_info',
		'agpu_vr_generic_event',
		'agpu_immediate_renderer_material_classic',
		'agpu_texture',
		'agpu_quaternionf',
		'agpu_string_length',
		'agpu_device_open_info',
		'agpu_sampler_description',
		'agpu_buffer_mapping_flags',
		'agpu_size2d',
		'agpu_renderpass_description',
		'agpu_vector2f',
		'agpu_cstring_buffer',
		'agpu_enum',
		'agpu_shader_binding_type',
		'agpu_shader_type',
		'agpu_vr_tracked_device_class',
		'agpu_renderpass_attachment_action',
		'agpu_vr_event_type',
		'agpu_depth_stencil_value',
		'agpu_immediate_renderer_light_classic',
		'agpu_immediate_renderer_lighting_model',
		'agpu_immediate_renderer_material',
		'agpu_state_tracker_cache',
		'agpu_inheritance_info',
		'agpu_color4f',
		'agpu_float',
		'agpu_texture_view',
		'agpu_shader',
		'agpu_double',
		'agpu_blending_factor',
		'agpu_state_tracker',
		'agpu_renderpass_depth_stencil_description',
		'agpu_vr_eye',
		'agpu_swap_chain_presentation_mode',
		'agpu_primitive_topology',
		'agpu_byte',
		'agpu_vr_event_data',
		'agpu_buffer',
		'agpu_texture_view_description',
		'agpu_vertex_layout',
		'agpu_texture_aspect',
		'agpu_immediate_renderer',
		'agpu_draw_elements_command',
		'agpu_limit',
		'agpu_cull_mode',
		'agpu_shader_signature',
		'agpu_error',
		'agpu_uint',
		'agpu_immediate_renderer_light',
		'agpu_short',
		'agpu_compute_pipeline_builder',
		'agpu_int',
		'agpu_vr_controller_event',
		'agpu_access_flags',
		'agpu_vr_dual_analog_event',
		'agpu_frustum_tangents',
		'agpu_cstring',
		'agpu_vr_tracked_device_pose',
		'agpu_mapping_access',
		'agpu_buffer_image_copy_region',
		'agpu_sampler',
		'agpu_compare_function',
		'agpu_size',
		'agpu_device_open_flags',
		'agpu_texture_usage_mode_mask',
		'agpu_sbyte',
		'agpu_renderpass',
		'agpu_offline_shader_compiler',
		'agpu_renderpass_color_attachment_description',
		'agpu_pipeline_stage_flags',
		'agpu_polygon_mode',
		'agpu_swap_chain_flags',
		'agpu_bool',
		'agpu_pipeline_builder',
		'agpu_shader_signature_builder',
		'agpu_texture_format',
		'agpu_vr_dual_analog_which',
		'agpu_device',
		'agpu_feature',
		'agpu_face_winding',
		'agpu_shader_resource_binding',
		'agpu_vr_button',
		'agpu_ushort',
		'agpu_pipeline_state',
		'agpu_components_swizzle',
		'agpu_vr_system',
		'agpu_command_queue_type',
		'agpu_texture_type',
		'agpu_immediate_renderer_light_pbr',
		'agpu_vr_event',
		'agpu_texture_clear_value',
		'agpu_buffer_usage_mask',
		'agpu_pointer',
		'agpu_size3d',
		'agpu_texture_address_mode',
		'agpu_vector3f',
		'agpu_shader_language',
		'agpu_vector4f',
		'agpu_command_list',
		'agpu_string_buffer',
		'agpu_vertex_binding',
		'agpu_swap_chain',
		'agpu_fence',
		'agpu_platform',
		'agpu_command_list_type',
		'agpu_immediate_renderer_vertex_attribute',
		'agpu_bitfield',
		'agpu_region3d',
		'agpu_vr_tracked_device_role',
		'agpu_memory_heap_type',
		'agpu_subresource_range',
		'agpu_stencil_operation',
		'agpu_command_queue',
		'agpu_component_swizzle',
		'agpu_command_allocator',
		'agpu_texture_description',
		'agpu_string',
		'agpu_blending_operation',
		'agpu_render_buffer_bit',
		'agpu_immediate_renderer_material_metallic_roughness'
	],
	#superclass : #SharedPool,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'initialize' }
AGPUTypes class >> initialize [
"
	self initialize
"
	super initialize.

	agpu_matrix3x3f := AGPUMatrix3x3f.
	agpu_immediate_renderer_fog_mode := #int.
	agpu_field_type := #int.
	agpu_buffer_description := AGPUBufferDescription.
	agpu_matrix4x4f := AGPUMatrix4x4f.
	agpu_filter := #int.
	agpu_vertex_attrib_description := AGPUVertexAttribDescription.
	agpu_framebuffer := #'void'.
	agpu_swap_chain_create_info := AGPUSwapChainCreateInfo.
	agpu_vr_generic_event := AGPUVrGenericEvent.
	agpu_immediate_renderer_material_classic := AGPUImmediateRendererMaterialClassic.
	agpu_texture := #'void'.
	agpu_quaternionf := AGPUQuaternionf.
	agpu_string_length := #'int'.
	agpu_device_open_info := AGPUDeviceOpenInfo.
	agpu_sampler_description := AGPUSamplerDescription.
	agpu_buffer_mapping_flags := #int.
	agpu_size2d := AGPUSize2d.
	agpu_renderpass_description := AGPURenderpassDescription.
	agpu_vector2f := AGPUVector2f.
	agpu_cstring_buffer := #'char*'.
	agpu_enum := #'int'.
	agpu_shader_binding_type := #int.
	agpu_shader_type := #int.
	agpu_vr_tracked_device_class := #int.
	agpu_renderpass_attachment_action := #int.
	agpu_vr_event_type := #int.
	agpu_depth_stencil_value := AGPUDepthStencilValue.
	agpu_immediate_renderer_light_classic := AGPUImmediateRendererLightClassic.
	agpu_immediate_renderer_lighting_model := #int.
	agpu_immediate_renderer_material := AGPUImmediateRendererMaterial.
	agpu_state_tracker_cache := #'void'.
	agpu_inheritance_info := AGPUInheritanceInfo.
	agpu_color4f := AGPUColor4f.
	agpu_float := #'float'.
	agpu_texture_view := #'void'.
	agpu_shader := #'void'.
	agpu_double := #'double'.
	agpu_blending_factor := #int.
	agpu_state_tracker := #'void'.
	agpu_renderpass_depth_stencil_description := AGPURenderpassDepthStencilDescription.
	agpu_vr_eye := #int.
	agpu_swap_chain_presentation_mode := #int.
	agpu_primitive_topology := #int.
	agpu_byte := #'uchar'.
	agpu_vr_event_data := AGPUVrEventData.
	agpu_buffer := #'void'.
	agpu_texture_view_description := AGPUTextureViewDescription.
	agpu_vertex_layout := #'void'.
	agpu_texture_aspect := #int.
	agpu_immediate_renderer := #'void'.
	agpu_draw_elements_command := AGPUDrawElementsCommand.
	agpu_limit := #int.
	agpu_cull_mode := #int.
	agpu_shader_signature := #'void'.
	agpu_error := #int.
	agpu_uint := #'uint'.
	agpu_immediate_renderer_light := AGPUImmediateRendererLight.
	agpu_short := #'short'.
	agpu_compute_pipeline_builder := #'void'.
	agpu_int := #'int'.
	agpu_vr_controller_event := AGPUVrControllerEvent.
	agpu_access_flags := #int.
	agpu_vr_dual_analog_event := AGPUVrDualAnalogEvent.
	agpu_frustum_tangents := AGPUFrustumTangents.
	agpu_cstring := #'char*'.
	agpu_vr_tracked_device_pose := AGPUVrTrackedDevicePose.
	agpu_mapping_access := #int.
	agpu_buffer_image_copy_region := AGPUBufferImageCopyRegion.
	agpu_sampler := #'void'.
	agpu_compare_function := #int.
	agpu_size := #'uint'.
	agpu_device_open_flags := #int.
	agpu_texture_usage_mode_mask := #int.
	agpu_sbyte := #'char'.
	agpu_renderpass := #'void'.
	agpu_offline_shader_compiler := #'void'.
	agpu_renderpass_color_attachment_description := AGPURenderpassColorAttachmentDescription.
	agpu_pipeline_stage_flags := #int.
	agpu_polygon_mode := #int.
	agpu_swap_chain_flags := #int.
	agpu_bool := #'int'.
	agpu_pipeline_builder := #'void'.
	agpu_shader_signature_builder := #'void'.
	agpu_texture_format := #int.
	agpu_vr_dual_analog_which := #int.
	agpu_device := #'void'.
	agpu_feature := #int.
	agpu_face_winding := #int.
	agpu_shader_resource_binding := #'void'.
	agpu_vr_button := #int.
	agpu_ushort := #'ushort'.
	agpu_pipeline_state := #'void'.
	agpu_components_swizzle := AGPUComponentsSwizzle.
	agpu_vr_system := #'void'.
	agpu_command_queue_type := #int.
	agpu_texture_type := #int.
	agpu_immediate_renderer_light_pbr := AGPUImmediateRendererLightPbr.
	agpu_vr_event := AGPUVrEvent.
	agpu_texture_clear_value := AGPUTextureClearValue.
	agpu_buffer_usage_mask := #int.
	agpu_pointer := #'void*'.
	agpu_size3d := AGPUSize3d.
	agpu_texture_address_mode := #int.
	agpu_vector3f := AGPUVector3f.
	agpu_shader_language := #int.
	agpu_vector4f := AGPUVector4f.
	agpu_command_list := #'void'.
	agpu_string_buffer := #'byte*'.
	agpu_vertex_binding := #'void'.
	agpu_swap_chain := #'void'.
	agpu_fence := #'void'.
	agpu_platform := #'void'.
	agpu_command_list_type := #int.
	agpu_immediate_renderer_vertex_attribute := #int.
	agpu_bitfield := #'uint'.
	agpu_region3d := AGPURegion3d.
	agpu_vr_tracked_device_role := #int.
	agpu_memory_heap_type := #int.
	agpu_subresource_range := AGPUSubresourceRange.
	agpu_stencil_operation := #int.
	agpu_command_queue := #'void'.
	agpu_component_swizzle := #int.
	agpu_command_allocator := #'void'.
	agpu_texture_description := AGPUTextureDescription.
	agpu_string := #'byte*'.
	agpu_blending_operation := #int.
	agpu_render_buffer_bit := #int.
	agpu_immediate_renderer_material_metallic_roughness := AGPUImmediateRendererMaterialMetallicRoughness.
]

