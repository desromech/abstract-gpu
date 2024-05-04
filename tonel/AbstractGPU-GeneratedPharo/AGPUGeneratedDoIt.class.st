Class {
	#name : #AGPUGeneratedDoIt,
	#superclass : #Object,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeConstants [
	<script>
	AGPUTypes initialize.
	AGPUConstants initialize.
]

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeStructures [
	<script>
	AGPUDeviceOpenInfo rebuildFieldAccessors.
	AGPUSwapChainCreateInfo rebuildFieldAccessors.
	AGPUBufferDescription rebuildFieldAccessors.
	AGPUColor4f rebuildFieldAccessors.
	AGPUDepthStencilValue rebuildFieldAccessors.
	AGPUTextureClearValue rebuildFieldAccessors.
	AGPUTextureDescription rebuildFieldAccessors.
	AGPUComponentsSwizzle rebuildFieldAccessors.
	AGPUTextureSubresourceRange rebuildFieldAccessors.
	AGPUTextureSubresourceLevel rebuildFieldAccessors.
	AGPUTextureViewDescription rebuildFieldAccessors.
	AGPUDrawElementsCommand rebuildFieldAccessors.
	AGPUVertexAttribDescription rebuildFieldAccessors.
	AGPUSamplerDescription rebuildFieldAccessors.
	AGPURenderpassColorAttachmentDescription rebuildFieldAccessors.
	AGPURenderpassDepthStencilDescription rebuildFieldAccessors.
	AGPURenderpassDescription rebuildFieldAccessors.
	AGPUInheritanceInfo rebuildFieldAccessors.
	AGPUVector2f rebuildFieldAccessors.
	AGPUVector3f rebuildFieldAccessors.
	AGPUVector4f rebuildFieldAccessors.
	AGPUQuaternionf rebuildFieldAccessors.
	AGPUMatrix3x3f rebuildFieldAccessors.
	AGPUMatrix4x4f rebuildFieldAccessors.
	AGPUSize2d rebuildFieldAccessors.
	AGPUSize3d rebuildFieldAccessors.
	AGPUFrustumTangents rebuildFieldAccessors.
	AGPUOffset3d rebuildFieldAccessors.
	AGPURegion3d rebuildFieldAccessors.
	AGPUBufferImageCopyRegion rebuildFieldAccessors.
	AGPUImageCopyRegion rebuildFieldAccessors.
	AGPUVrTrackedDevicePose rebuildFieldAccessors.
	AGPUVrGenericEvent rebuildFieldAccessors.
	AGPUVrControllerEvent rebuildFieldAccessors.
	AGPUVrDualAnalogEvent rebuildFieldAccessors.
	AGPUVrEventData rebuildFieldAccessors.
	AGPUVrEvent rebuildFieldAccessors.
	AGPUVrRenderModelTexture rebuildFieldAccessors.
	AGPUVrRenderModelVertex rebuildFieldAccessors.
	AGPUVrRenderModel rebuildFieldAccessors.
	AGPUImmediateRendererLightClassic rebuildFieldAccessors.
	AGPUImmediateRendererLightPbr rebuildFieldAccessors.
	AGPUImmediateRendererLight rebuildFieldAccessors.
	AGPUImmediateRendererMaterialClassic rebuildFieldAccessors.
	AGPUImmediateRendererMaterialMetallicRoughness rebuildFieldAccessors.
	AGPUImmediateRendererMaterialFlatColor rebuildFieldAccessors.
	AGPUImmediateRendererMaterial rebuildFieldAccessors.
]

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeBindings [
	<script>
	self initializeConstants.
	self initializeStructures.
]

