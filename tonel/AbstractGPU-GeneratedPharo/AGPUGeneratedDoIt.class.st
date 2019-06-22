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
	AGPUTextureDescription rebuildFieldAccessors.
	AGPUComponentsSwizzle rebuildFieldAccessors.
	AGPUSubresourceRange rebuildFieldAccessors.
	AGPUTextureViewDescription rebuildFieldAccessors.
	AGPUDrawElementsCommand rebuildFieldAccessors.
	AGPUVertexAttribDescription rebuildFieldAccessors.
	AGPUColor4f rebuildFieldAccessors.
	AGPUDepthStencilValue rebuildFieldAccessors.
	AGPUSamplerDescription rebuildFieldAccessors.
	AGPURenderpassColorAttachmentDescription rebuildFieldAccessors.
	AGPURenderpassDepthStencilDescription rebuildFieldAccessors.
	AGPURenderpassDescription rebuildFieldAccessors.
	AGPUInheritanceInfo rebuildFieldAccessors.
	AGPUVector3f rebuildFieldAccessors.
	AGPUVector4f rebuildFieldAccessors.
	AGPUQuaternionf rebuildFieldAccessors.
	AGPUMatrix3x3f rebuildFieldAccessors.
	AGPUMatrix4x4f rebuildFieldAccessors.
	AGPUSize2d rebuildFieldAccessors.
	AGPUSize3d rebuildFieldAccessors.
	AGPUFrustumTangents rebuildFieldAccessors.
	AGPURegion3d rebuildFieldAccessors.
	AGPUVrTrackedDevicePose rebuildFieldAccessors.
	AGPUVrGenericEvent rebuildFieldAccessors.
	AGPUVrControllerEvent rebuildFieldAccessors.
	AGPUVrDualAnalogEvent rebuildFieldAccessors.
	AGPUVrEventData rebuildFieldAccessors.
	AGPUVrEvent rebuildFieldAccessors.
	AGPUImmediateRendererLight rebuildFieldAccessors.
]

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeBindings [
	<script>
	self initializeConstants.
	self initializeStructures.
]

