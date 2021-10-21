Class {
	#name : #AGPUStateTracker,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #wrappers }
AGPUStateTracker >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> beginRecordingCommands [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginRecordingCommands_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> beginRenderPass: renderpass framebuffer: framebuffer bundle_content: bundle_content [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginRenderPass_state_tracker: (self validHandle) renderpass: (self validHandleOf: renderpass) framebuffer: (self validHandleOf: framebuffer) bundle_content: bundle_content.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> bufferMemoryBarrier: buffer source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bufferMemoryBarrier_state_tracker: (self validHandle) buffer: (self validHandleOf: buffer) source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> copyBuffer: source_buffer source_offset: source_offset dest_buffer: dest_buffer dest_offset: dest_offset copy_size: copy_size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyBuffer_state_tracker: (self validHandle) source_buffer: (self validHandleOf: source_buffer) source_offset: source_offset dest_buffer: (self validHandleOf: dest_buffer) dest_offset: dest_offset copy_size: copy_size.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> copyBufferToTexture: buffer texture: texture copy_region: copy_region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyBufferToTexture_state_tracker: (self validHandle) buffer: (self validHandleOf: buffer) texture: (self validHandleOf: texture) copy_region: copy_region.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> copyTexture: source_texture dest_texture: dest_texture copy_region: copy_region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyTexture_state_tracker: (self validHandle) source_texture: (self validHandleOf: source_texture) dest_texture: (self validHandleOf: dest_texture) copy_region: copy_region.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> copyTextureToBuffer: texture buffer: buffer copy_region: copy_region [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance copyTextureToBuffer_state_tracker: (self validHandle) texture: (self validHandleOf: texture) buffer: (self validHandleOf: buffer) copy_region: copy_region.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> dispatchCompute: group_count_x group_count_y: group_count_y group_count_z: group_count_z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance dispatchCompute_state_tracker: (self validHandle) group_count_x: group_count_x group_count_y: group_count_y group_count_z: group_count_z.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> dispatchComputeIndirect: offset [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance dispatchComputeIndirect_state_tracker: (self validHandle) offset: offset.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> drawArrays: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawArrays_state_tracker: (self validHandle) vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> drawArraysIndirect: offset drawcount: drawcount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawArraysIndirect_state_tracker: (self validHandle) offset: offset drawcount: drawcount.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> drawElements: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawElements_state_tracker: (self validHandle) index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> drawElementsIndirect: offset drawcount: drawcount [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawElementsIndirect_state_tracker: (self validHandle) offset: offset drawcount: drawcount.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> endRecordingAndFlushCommands [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endRecordingAndFlushCommands_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> endRecordingCommands [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endRecordingCommands_state_tracker: (self validHandle).
	^ AGPUCommandList forHandle: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> endRenderPass [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endRenderPass_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> executeBundle: bundle [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance executeBundle_state_tracker: (self validHandle) bundle: (self validHandleOf: bundle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> memoryBarrier: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance memoryBarrier_state_tracker: (self validHandle) source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> popBufferTransitionBarrier [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance popBufferTransitionBarrier_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> popTextureTransitionBarrier [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance popTextureTransitionBarrier_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> pushBufferTransitionBarrier: buffer old_usage: old_usage new_usage: new_usage [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushBufferTransitionBarrier_state_tracker: (self validHandle) buffer: (self validHandleOf: buffer) old_usage: old_usage new_usage: new_usage.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> pushConstants: offset size: size values: values [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushConstants_state_tracker: (self validHandle) offset: offset size: size values: values.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> pushTextureTransitionBarrier: texture old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushTextureTransitionBarrier_state_tracker: (self validHandle) texture: (self validHandleOf: texture) old_usage: old_usage new_usage: new_usage subresource_range: subresource_range.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> reset [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance reset_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> resetComputePipeline [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resetComputePipeline_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> resetGraphicsPipeline [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resetGraphicsPipeline_state_tracker: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> resolveFramebuffer: destFramebuffer sourceFramebuffer: sourceFramebuffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resolveFramebuffer_state_tracker: (self validHandle) destFramebuffer: (self validHandleOf: destFramebuffer) sourceFramebuffer: (self validHandleOf: sourceFramebuffer).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> resolveTexture: sourceTexture sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: destTexture destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance resolveTexture_state_tracker: (self validHandle) sourceTexture: (self validHandleOf: sourceTexture) sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: (self validHandleOf: destTexture) destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setBlendFunction: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setBlendFunction_state_tracker: (self validHandle) renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setBlendState: renderTargetMask enabled: enabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setBlendState_state_tracker: (self validHandle) renderTargetMask: renderTargetMask enabled: enabled.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setColorMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setColorMask_state_tracker: (self validHandle) renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setComputeStage: shader entryPoint: entryPoint [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setComputeStage_state_tracker: (self validHandle) shader: (self validHandleOf: shader) entryPoint: entryPoint.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setCullMode: mode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setCullMode_state_tracker: (self validHandle) mode: mode.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setDepthBias: constant_factor clamp: clamp slope_factor: slope_factor [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthBias_state_tracker: (self validHandle) constant_factor: constant_factor clamp: clamp slope_factor: slope_factor.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setDepthState: enabled writeMask: writeMask function: function [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthState_state_tracker: (self validHandle) enabled: enabled writeMask: writeMask function: function.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setFragmentStage: shader entryPoint: entryPoint [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setFragmentStage_state_tracker: (self validHandle) shader: (self validHandleOf: shader) entryPoint: entryPoint.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setFrontFace: winding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setFrontFace_state_tracker: (self validHandle) winding: winding.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setGeometryStage: shader entryPoint: entryPoint [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setGeometryStage_state_tracker: (self validHandle) shader: (self validHandleOf: shader) entryPoint: entryPoint.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setPolygonMode: mode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setPolygonMode_state_tracker: (self validHandle) mode: mode.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setPrimitiveType: type [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setPrimitiveType_state_tracker: (self validHandle) type: type.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setSampleDescription: sample_count sample_quality: sample_quality [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setSampleDescription_state_tracker: (self validHandle) sample_count: sample_count sample_quality: sample_quality.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setScissor: x y: y w: w h: h [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setScissor_state_tracker: (self validHandle) x: x y: y w: w h: h.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setShaderSignature: signature [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setShaderSignature_state_tracker: (self validHandle) signature: (self validHandleOf: signature).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setStencilBackFace: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilBackFace_state_tracker: (self validHandle) stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setStencilFrontFace: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilFrontFace_state_tracker: (self validHandle) stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setStencilReference: reference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilReference_state_tracker: (self validHandle) reference: reference.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setStencilState: enabled writeMask: writeMask readMask: readMask [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilState_state_tracker: (self validHandle) enabled: enabled writeMask: writeMask readMask: readMask.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setTessellationControlStage: shader entryPoint: entryPoint [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setTessellationControlStage_state_tracker: (self validHandle) shader: (self validHandleOf: shader) entryPoint: entryPoint.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setTessellationEvaluationStage: shader entryPoint: entryPoint [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setTessellationEvaluationStage_state_tracker: (self validHandle) shader: (self validHandleOf: shader) entryPoint: entryPoint.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setVertexLayout: layout [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setVertexLayout_state_tracker: (self validHandle) layout: (self validHandleOf: layout).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setVertexStage: shader entryPoint: entryPoint [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setVertexStage_state_tracker: (self validHandle) shader: (self validHandleOf: shader) entryPoint: entryPoint.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> setViewport: x y: y w: w h: h [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setViewport_state_tracker: (self validHandle) x: x y: y w: w h: h.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> textureMemoryBarrier: texture source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance textureMemoryBarrier_state_tracker: (self validHandle) texture: (self validHandleOf: texture) source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses old_usage: old_usage new_usage: new_usage subresource_range: subresource_range.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useComputeDispatchIndirectBuffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useComputeDispatchIndirectBuffer_state_tracker: (self validHandle) buffer: (self validHandleOf: buffer).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useComputeShaderResources: binding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useComputeShaderResources_state_tracker: (self validHandle) binding: (self validHandleOf: binding).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useComputeShaderResourcesInSlot: binding slot: slot [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useComputeShaderResourcesInSlot_state_tracker: (self validHandle) binding: (self validHandleOf: binding) slot: slot.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useDrawIndirectBuffer: draw_buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useDrawIndirectBuffer_state_tracker: (self validHandle) draw_buffer: (self validHandleOf: draw_buffer).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useIndexBuffer: index_buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useIndexBuffer_state_tracker: (self validHandle) index_buffer: (self validHandleOf: index_buffer).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useIndexBufferAt: index_buffer offset: offset index_size: index_size [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useIndexBufferAt_state_tracker: (self validHandle) index_buffer: (self validHandleOf: index_buffer) offset: offset index_size: index_size.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useShaderResources: binding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useShaderResources_state_tracker: (self validHandle) binding: (self validHandleOf: binding).
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useShaderResourcesInSlot: binding slot: slot [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useShaderResourcesInSlot_state_tracker: (self validHandle) binding: (self validHandleOf: binding) slot: slot.
	self checkErrorCode: resultValue_
]

{ #category : #wrappers }
AGPUStateTracker >> useVertexBinding: vertex_binding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance useVertexBinding_state_tracker: (self validHandle) vertex_binding: (self validHandleOf: vertex_binding).
	self checkErrorCode: resultValue_
]
