Class {
	#name : #AGPUCommandList,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUCommandList >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_command_list: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_command_list: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> setShaderSignature: signature [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setShaderSignature_command_list: (self validHandle) signature: (self validHandleOf: signature).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> setViewport: x y: y w: w h: h [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setViewport_command_list: (self validHandle) x: x y: y w: w h: h.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> setScissor: x y: y w: w h: h [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setScissor_command_list: (self validHandle) x: x y: y w: w h: h.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> usePipelineState: pipeline [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance usePipelineState_command_list: (self validHandle) pipeline: (self validHandleOf: pipeline).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useVertexBinding: vertex_binding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useVertexBinding_command_list: (self validHandle) vertex_binding: (self validHandleOf: vertex_binding).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useIndexBuffer: index_buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useIndexBuffer_command_list: (self validHandle) index_buffer: (self validHandleOf: index_buffer).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useIndexBufferAt: index_buffer offset: offset index_size: index_size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useIndexBufferAt_command_list: (self validHandle) index_buffer: (self validHandleOf: index_buffer) offset: offset index_size: index_size.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useDrawIndirectBuffer: draw_buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useDrawIndirectBuffer_command_list: (self validHandle) draw_buffer: (self validHandleOf: draw_buffer).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useComputeDispatchIndirectBuffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useComputeDispatchIndirectBuffer_command_list: (self validHandle) buffer: (self validHandleOf: buffer).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useShaderResources: binding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useShaderResources_command_list: (self validHandle) binding: (self validHandleOf: binding).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> useComputeShaderResources: binding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useComputeShaderResources_command_list: (self validHandle) binding: (self validHandleOf: binding).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> drawArrays: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawArrays_command_list: (self validHandle) vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> drawArraysIndirect: offset drawcount: drawcount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawArraysIndirect_command_list: (self validHandle) offset: offset drawcount: drawcount.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> drawElements: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawElements_command_list: (self validHandle) index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> drawElementsIndirect: offset drawcount: drawcount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawElementsIndirect_command_list: (self validHandle) offset: offset drawcount: drawcount.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> dispatchCompute: group_count_x group_count_y: group_count_y group_count_z: group_count_z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance dispatchCompute_command_list: (self validHandle) group_count_x: group_count_x group_count_y: group_count_y group_count_z: group_count_z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> dispatchComputeIndirect: offset [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance dispatchComputeIndirect_command_list: (self validHandle) offset: offset.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> setStencilReference: reference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilReference_command_list: (self validHandle) reference: reference.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> executeBundle: bundle [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance executeBundle_command_list: (self validHandle) bundle: (self validHandleOf: bundle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> close [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance close_command_list: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> reset: allocator initial_pipeline_state: initial_pipeline_state [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance reset_command_list: (self validHandle) allocator: (self validHandleOf: allocator) initial_pipeline_state: (self validHandleOf: initial_pipeline_state).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> resetBundle: allocator initial_pipeline_state: initial_pipeline_state inheritance_info: inheritance_info [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resetBundle_command_list: (self validHandle) allocator: (self validHandleOf: allocator) initial_pipeline_state: (self validHandleOf: initial_pipeline_state) inheritance_info: inheritance_info.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> beginRenderPass: renderpass framebuffer: framebuffer bundle_content: bundle_content [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginRenderPass_command_list: (self validHandle) renderpass: (self validHandleOf: renderpass) framebuffer: (self validHandleOf: framebuffer) bundle_content: bundle_content.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> endRenderPass [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endRenderPass_command_list: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> resolveFramebuffer: destFramebuffer sourceFramebuffer: sourceFramebuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resolveFramebuffer_command_list: (self validHandle) destFramebuffer: (self validHandleOf: destFramebuffer) sourceFramebuffer: (self validHandleOf: sourceFramebuffer).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> resolveTexture: sourceTexture sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: destTexture destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resolveTexture_command_list: (self validHandle) sourceTexture: (self validHandleOf: sourceTexture) sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: (self validHandleOf: destTexture) destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> pushConstants: offset size: size values: values [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushConstants_command_list: (self validHandle) offset: offset size: size values: values.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> memoryBarrier: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance memoryBarrier_command_list: (self validHandle) source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> bufferMemoryBarrier: buffer source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bufferMemoryBarrier_command_list: (self validHandle) buffer: (self validHandleOf: buffer) source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> textureMemoryBarrier: texture source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance textureMemoryBarrier_command_list: (self validHandle) texture: (self validHandleOf: texture) source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses old_usage: old_usage new_usage: new_usage subresource_range: subresource_range.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> pushBufferTransitionBarrier: buffer old_usage: old_usage new_usage: new_usage [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushBufferTransitionBarrier_command_list: (self validHandle) buffer: (self validHandleOf: buffer) old_usage: old_usage new_usage: new_usage.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> pushTextureTransitionBarrier: texture old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushTextureTransitionBarrier_command_list: (self validHandle) texture: (self validHandleOf: texture) old_usage: old_usage new_usage: new_usage subresource_range: subresource_range.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> popBufferTransitionBarrier [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance popBufferTransitionBarrier_command_list: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> popTextureTransitionBarrier [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance popTextureTransitionBarrier_command_list: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> copyBuffer: source_buffer source_offset: source_offset dest_buffer: dest_buffer dest_offset: dest_offset copy_size: copy_size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyBuffer_command_list: (self validHandle) source_buffer: (self validHandleOf: source_buffer) source_offset: source_offset dest_buffer: (self validHandleOf: dest_buffer) dest_offset: dest_offset copy_size: copy_size.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> copyBufferToTexture: buffer texture: texture copy_region: copy_region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyBufferToTexture_command_list: (self validHandle) buffer: (self validHandleOf: buffer) texture: (self validHandleOf: texture) copy_region: copy_region.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> copyTextureToBuffer: texture buffer: buffer copy_region: copy_region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyTextureToBuffer_command_list: (self validHandle) texture: (self validHandleOf: texture) buffer: (self validHandleOf: buffer) copy_region: copy_region.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUCommandList >> copyTexture: source_texture dest_texture: dest_texture copy_region: copy_region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyTexture_command_list: (self validHandle) source_texture: (self validHandleOf: source_texture) dest_texture: (self validHandleOf: dest_texture) copy_region: copy_region.
	self checkErrorCode: resultValue_
]

