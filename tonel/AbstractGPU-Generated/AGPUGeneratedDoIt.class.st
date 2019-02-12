Class {
	#name : #AGPUGeneratedDoIt,
	#superclass : #Object,
	#category : 'AbstractGPU-Generated'
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
	AGPUSize3d rebuildFieldAccessors.
	AGPURegion3d rebuildFieldAccessors.
]

{ #category : #'initialization' }
AGPUGeneratedDoIt class >> initializeBindings [
	<script>
	self initializeConstants.
	self initializeStructures.
]

