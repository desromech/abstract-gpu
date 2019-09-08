Class {
	#name : #AGPUImmediateRenderer,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'wrappers' }
AGPUImmediateRenderer >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> beginRendering: state_tracker [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginRendering_immediate_renderer: (self validHandle) state_tracker: (self validHandleOf: state_tracker).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> endRendering [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endRendering_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setBlendState: renderTargetMask enabled: enabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setBlendState_immediate_renderer: (self validHandle) renderTargetMask: renderTargetMask enabled: enabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setBlendFunction: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setBlendFunction_immediate_renderer: (self validHandle) renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setColorMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setColorMask_immediate_renderer: (self validHandle) renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setFrontFace: winding [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setFrontFace_immediate_renderer: (self validHandle) winding: winding.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setCullMode: mode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setCullMode_immediate_renderer: (self validHandle) mode: mode.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setDepthBias: constant_factor clamp: clamp slope_factor: slope_factor [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthBias_immediate_renderer: (self validHandle) constant_factor: constant_factor clamp: clamp slope_factor: slope_factor.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setDepthState: enabled writeMask: writeMask function: function [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setDepthState_immediate_renderer: (self validHandle) enabled: enabled writeMask: writeMask function: function.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setPolygonMode: mode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setPolygonMode_immediate_renderer: (self validHandle) mode: mode.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setStencilState: enabled writeMask: writeMask readMask: readMask [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilState_immediate_renderer: (self validHandle) enabled: enabled writeMask: writeMask readMask: readMask.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setStencilFrontFace: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilFrontFace_immediate_renderer: (self validHandle) stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setStencilBackFace: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilBackFace_immediate_renderer: (self validHandle) stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setViewport: x y: y w: w h: h [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setViewport_immediate_renderer: (self validHandle) x: x y: y w: w h: h.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setScissor: x y: y w: w h: h [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setScissor_immediate_renderer: (self validHandle) x: x y: y w: w h: h.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setStencilReference: reference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setStencilReference_immediate_renderer: (self validHandle) reference: reference.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> projectionMatrixMode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance projectionMatrixMode_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> modelViewMatrixMode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance modelViewMatrixMode_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> textureMatrixMode [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance textureMatrixMode_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> loadIdentity [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance loadIdentity_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> pushMatrix [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance pushMatrix_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> popMatrix [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance popMatrix_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> loadMatrix: elements [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance loadMatrix_immediate_renderer: (self validHandle) elements: elements.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> loadTransposeMatrix: elements [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance loadTransposeMatrix_immediate_renderer: (self validHandle) elements: elements.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> multiplyMatrix: elements [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance multiplyMatrix_immediate_renderer: (self validHandle) elements: elements.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> multiplyTransposeMatrix: elements [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance multiplyTransposeMatrix_immediate_renderer: (self validHandle) elements: elements.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> ortho: left right: right bottom: bottom top: top near: near far: far [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance ortho_immediate_renderer: (self validHandle) left: left right: right bottom: bottom top: top near: near far: far.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> frustum: left right: right bottom: bottom top: top near: near far: far [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance frustum_immediate_renderer: (self validHandle) left: left right: right bottom: bottom top: top near: near far: far.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> perspective: fovy aspect: aspect near: near far: far [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance perspective_immediate_renderer: (self validHandle) fovy: fovy aspect: aspect near: near far: far.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> rotate: angle x: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance rotate_immediate_renderer: (self validHandle) angle: angle x: x y: y z: z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> translate: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance translate_immediate_renderer: (self validHandle) x: x y: y z: z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> scale: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance scale_immediate_renderer: (self validHandle) x: x y: y z: z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setFlatShading: enabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setFlatShading_immediate_renderer: (self validHandle) enabled: enabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setLightingEnabled: enabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setLightingEnabled_immediate_renderer: (self validHandle) enabled: enabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> clearLights [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance clearLights_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setAmbientLighting: r g: g b: b a: a [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setAmbientLighting_immediate_renderer: (self validHandle) r: r g: g b: b a: a.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setLight: index enabled: enabled state: state [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setLight_immediate_renderer: (self validHandle) index: index enabled: enabled state: state.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setMaterial: state [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setMaterial_immediate_renderer: (self validHandle) state: state.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setTexturingEnabled: enabled [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setTexturingEnabled_immediate_renderer: (self validHandle) enabled: enabled.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> bindTexture: texture [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance bindTexture_immediate_renderer: (self validHandle) texture: (self validHandleOf: texture).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> beginPrimitives: type [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginPrimitives_immediate_renderer: (self validHandle) type: type.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> endPrimitives [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endPrimitives_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> color: r g: g b: b a: a [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance color_immediate_renderer: (self validHandle) r: r g: g b: b a: a.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> texcoord: x y: y [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance texcoord_immediate_renderer: (self validHandle) x: x y: y.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> normal: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance normal_immediate_renderer: (self validHandle) x: x y: y z: z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> vertex: x y: y z: z [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance vertex_immediate_renderer: (self validHandle) x: x y: y z: z.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> beginMeshWithVertices: vertexCount stride: stride elementCount: elementCount vertices: vertices [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance beginMeshWithVertices_immediate_renderer: (self validHandle) vertexCount: vertexCount stride: stride elementCount: elementCount vertices: vertices.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setCurrentMeshColors: stride elementCount: elementCount colors: colors [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setCurrentMeshColors_immediate_renderer: (self validHandle) stride: stride elementCount: elementCount colors: colors.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setCurrentMeshNormals: stride elementCount: elementCount normals: normals [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setCurrentMeshNormals_immediate_renderer: (self validHandle) stride: stride elementCount: elementCount normals: normals.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> setCurrentMeshTexCoords: stride elementCount: elementCount texcoords: texcoords [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setCurrentMeshTexCoords_immediate_renderer: (self validHandle) stride: stride elementCount: elementCount texcoords: texcoords.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> drawElementsWithIndices: mode indices: indices index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance drawElementsWithIndices_immediate_renderer: (self validHandle) mode: mode indices: indices index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUImmediateRenderer >> endMesh [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance endMesh_immediate_renderer: (self validHandle).
	self checkErrorCode: resultValue_
]

