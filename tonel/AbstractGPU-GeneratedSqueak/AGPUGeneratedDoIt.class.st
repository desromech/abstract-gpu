Class {
	#name : #AGPUGeneratedDoIt,
	#superclass : #Object,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeConstants [
	<script>
	AGPUConstants initialize.
]

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeStructures [
	<script>
	AGPUDeviceOpenInfo defineFields.
	AGPUSwapChainCreateInfo defineFields.
	AGPUBufferDescription defineFields.
	AGPUColor4f defineFields.
	AGPUDepthStencilValue defineFields.
	AGPUTextureClearValue defineFields.
	AGPUTextureDescription defineFields.
	AGPUComponentsSwizzle defineFields.
	AGPUSubresourceRange defineFields.
	AGPUTextureViewDescription defineFields.
	AGPUDrawElementsCommand defineFields.
	AGPUVertexAttribDescription defineFields.
	AGPUSamplerDescription defineFields.
	AGPURenderpassColorAttachmentDescription defineFields.
	AGPURenderpassDepthStencilDescription defineFields.
	AGPURenderpassDescription defineFields.
	AGPUInheritanceInfo defineFields.
	AGPUVector2f defineFields.
	AGPUVector3f defineFields.
	AGPUVector4f defineFields.
	AGPUQuaternionf defineFields.
	AGPUMatrix3x3f defineFields.
	AGPUMatrix4x4f defineFields.
	AGPUSize2d defineFields.
	AGPUSize3d defineFields.
	AGPUFrustumTangents defineFields.
	AGPURegion3d defineFields.
	AGPUBufferImageCopyRegion defineFields.
	AGPUVrTrackedDevicePose defineFields.
	AGPUVrGenericEvent defineFields.
	AGPUVrControllerEvent defineFields.
	AGPUVrDualAnalogEvent defineFields.
	AGPUVrEventData defineFields.
	AGPUVrEvent defineFields.
	AGPUImmediateRendererLightClassic defineFields.
	AGPUImmediateRendererLightPbr defineFields.
	AGPUImmediateRendererLight defineFields.
	AGPUImmediateRendererMaterialClassic defineFields.
	AGPUImmediateRendererMaterialMetallicRoughness defineFields.
	AGPUImmediateRendererMaterialFlatColor defineFields.
	AGPUImmediateRendererMaterial defineFields.
]

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeBindings [
	<script>
	self initializeConstants.
	self initializeStructures.
]

