Class {
	#name : #AGPUSampleMesh,
	#superclass : #Object,
	#instVars : [
		'device',
		'vertices',
		'submeshes',
		'indices',
		'baseVertex',
		'vertexBuffer',
		'indexBuffer',
		'vertexBinding',
		'encodedPositions',
		'encodedNormals',
		'encodedTexcoords',
		'encodedColors',
		'encodedIndices',
		'encodedTangents4',
		'vertexLayout',
		'encodedVertexBuffer',
		'encodedVertexBufferOffsets'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #'instance creation' }
AGPUSampleMesh class >> for: device [
	^ self new device: device; yourself.
]

{ #category : #adding }
AGPUSampleMesh >> addCubeMinX: minX y: minY z: minZ maxX: maxX y: maxY z: maxZ [
	self
		"Left"
		beginTriangles;
		addVertexP: { minX . minY . minZ } N: #(-1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(0.0 1.0);
		addVertexP: { minX . maxY . minZ } N: #(-1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(1.0 1.0);
		addVertexP: { minX . maxY . maxZ } N: #(-1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(1.0 0.0);
		addVertexP: { minX . minY . maxZ } N: #(-1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(0.0 0.0);
		addTriangleI1: 1 i2: 0 i3: 2;
		addTriangleI1: 3 i2: 2 i3: 0;

		"Right"
		beginTriangles;
		addVertexP: { maxX . minY . minZ } N: #(1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(0.0 1.0);
		addVertexP: { maxX . maxY . minZ } N: #(1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(1.0 1.0);
		addVertexP: { maxX . maxY . maxZ } N: #(1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(1.0 0.0);
		addVertexP: { maxX . minY . maxZ } N: #(1.0 0.0 0.0) T4: #(0.0 1.0 0.0 1.0) TC: #(0.0 0.0);
		addTriangleI1: 0 i2: 1 i3: 2;
		addTriangleI1: 2 i2: 3 i3: 0;

		"Top"
		beginTriangles;
		addVertexP: { minX . maxY . minZ } N: #(0.0 1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(0.0 1.0);
		addVertexP: { maxX . maxY . minZ } N: #(0.0 1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(1.0 1.0);
		addVertexP: { maxX . maxY . maxZ } N: #(0.0 1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(1.0 0.0);
		addVertexP: { minX . maxY . maxZ } N: #(0.0 1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(0.0 0.0);
		addTriangleI1: 1 i2: 0 i3: 2;
		addTriangleI1: 3 i2: 2 i3: 0;

		"Bottom"
		beginTriangles;
		addVertexP: { minX . minY . minZ } N: #(0.0 -1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(0.0 1.0);
		addVertexP: { maxX . minY . minZ } N: #(0.0 -1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(1.0 1.0);
		addVertexP: { maxX . minY . maxZ } N: #(0.0 -1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(1.0 0.0);
		addVertexP: { minX . minY . maxZ } N: #(0.0 -1.0 0.0) T4: #(1.0 0.0 0.0 1.0) TC: #(0.0 0.0);
		addTriangleI1: 0 i2: 1 i3: 2;
		addTriangleI1: 2 i2: 3 i3: 0;

		"Back"
		beginTriangles;
		addVertexP: { minX . minY . minZ } N: #(0.0 0.0 -1.0) TC: #(0.0 1.0);
		addVertexP: { maxX . minY . minZ } N: #(0.0 0.0 -1.0) TC: #(1.0 1.0);
		addVertexP: { maxX . maxY . minZ } N: #(0.0 0.0 -1.0) TC: #(1.0 0.0);
		addVertexP: { minX . maxY . minZ } N: #(0.0 0.0 -1.0) TC: #(0.0 0.0);
		addTriangleI1: 1 i2: 0 i3: 2;
		addTriangleI1: 3 i2: 2 i3: 0;

		"Front"
		beginTriangles;
		addVertexP: { minX . minY . maxZ } N: #(0.0 0.0 1.0) TC: #(0.0 1.0);
		addVertexP: { maxX . minY . maxZ } N: #(0.0 0.0 1.0) TC: #(1.0 1.0);
		addVertexP: { maxX . maxY . maxZ } N: #(0.0 0.0 1.0) TC: #(1.0 0.0);
		addVertexP: { minX . maxY . maxZ } N: #(0.0 0.0 1.0) TC: #(0.0 0.0);
		addTriangleI1: 0 i2: 1 i3: 2;
		addTriangleI1: 2 i2: 3 i3: 0;

		finishLastSubmesh
]

{ #category : #adding }
AGPUSampleMesh >> addTriangleI1: i1 i2: i2 i3: i3 [
	indices
		add: i1 + baseVertex;
		add: i2 + baseVertex;
		add: i3 + baseVertex
]

{ #category : #adding }
AGPUSampleMesh >> addVertexP: position N: normal T4: tangent4 TC: texcoord [
	vertices add: (AGPUSampleMeshVertex new
		position: position;
		color: #(1.0 1.0 1.0 1.0);
		normal: normal;
		texcoord: texcoord;
		tangent4: tangent4;
		yourself)
]

{ #category : #adding }
AGPUSampleMesh >> addVertexP: position N: normal TC: texcoord [
	vertices add: (AGPUSampleMeshVertex new
		position: position;
		color: #(1.0 1.0 1.0 1.0);
		normal: normal;
		texcoord: texcoord;
		tangent4: #(1 0 0 1);
		yourself)
]

{ #category : #drawing }
AGPUSampleMesh >> beginDrawingUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer [
	vertices ifEmpty: [ ^ self ].
	indices ifEmpty: [ ^ self ].
	
	useExplicitBuffers ifTrue: [ 
		immediateRenderer
			beginMeshWithVertexBinding: self vertexLayout vertices: self vertexBinding;
			useIndexBuffer: self indexBuffer
	] ifFalse: [ 
		immediateRenderer
			beginMeshWithVertices: vertices size stride: 12 elementCount: 3 vertices: self encodedPositions;
			setCurrentMeshColors: 16 elementCount: 4 colors: self encodedColors;
			setCurrentMeshNormals: 12 elementCount: 3 normals: self encodedNormals;
			setCurrentMeshTexCoords: 8 elementCount: 2 texcoords: self encodedTexcoords
	].
	

]

{ #category : #'mesh building' }
AGPUSampleMesh >> beginPrimitives: primitiveTopology [
	baseVertex := vertices size.
	(submeshes isEmpty not and: [ submeshes last primitiveTopology == primitiveTopology ]) ifTrue: [ ^ self ].
	self finishLastSubmesh.
	submeshes add: (AGPUSampleSubMesh new
		primitiveTopology: primitiveTopology;
		firstIndex: indices size;
		yourself)
]

{ #category : #'mesh building' }
AGPUSampleMesh >> beginTriangles [
	self beginPrimitives: AGPU_TRIANGLES
]

{ #category : #accessing }
AGPUSampleMesh >> device [

	^ device
]

{ #category : #accessing }
AGPUSampleMesh >> device: anObject [

	device := anObject
]

{ #category : #drawing }
AGPUSampleMesh >> drawUsingExplicitBuffers: useExplicitBuffers withImmediateRenderer: immediateRenderer [
	vertices ifEmpty: [ ^ self ].
	indices ifEmpty: [ ^ self ].
	
	submeshes do: [ :each |
		useExplicitBuffers ifTrue: [
			immediateRenderer
				setPrimitiveType: each primitiveTopology;
				drawElements: each indexCount instance_count: 1
					first_index: each firstIndex base_vertex: 0 base_instance: 0
		] ifFalse: [ 
			immediateRenderer
				drawElementsWithIndices: each primitiveTopology
				indices: self encodedIndices index_count: each indexCount
				instance_count: 1 first_index: each firstIndex
				base_vertex: 0 base_instance: 0
		].
	]
]

{ #category : #private }
AGPUSampleMesh >> encodeFloatComponent: componentBlock count: componentCount [
	| encodedData destIndex |
	encodedData := ByteArray new: vertices size * componentCount * 4.
	destIndex := 1.
	vertices do: [ :vertex |
		(componentBlock value: vertex) do: [ :coord |
			encodedData floatAt: destIndex put: coord.
			destIndex := destIndex + 4
		].
	].
	^ encodedData
]

{ #category : #private }
AGPUSampleMesh >> encodedColors [
	^ encodedColors ifNil: [
		encodedColors := self encodeFloatComponent: [:v | v color] count: 4
	]
]

{ #category : #private }
AGPUSampleMesh >> encodedIndices [
	^ encodedIndices ifNil: [
		encodedIndices := indices asWordArray shallowCopy.
		ByteArray adoptInstance: encodedIndices.
		encodedIndices
	]
]

{ #category : #private }
AGPUSampleMesh >> encodedNormals [
	^ encodedNormals ifNil: [
		encodedNormals := self encodeFloatComponent: [:v | v normal] count: 3
	]
]

{ #category : #private }
AGPUSampleMesh >> encodedPositions [
	^ encodedPositions ifNil: [
		encodedPositions := self encodeFloatComponent: [:v | v position] count: 3
	]
]

{ #category : #private }
AGPUSampleMesh >> encodedTangents4 [
	^ encodedTangents4 ifNil: [
		encodedTangents4 := self encodeFloatComponent: [:v | v tangent4] count: 4
	]
]

{ #category : #private }
AGPUSampleMesh >> encodedTexcoords [
	^ encodedTexcoords ifNil: [
		encodedTexcoords := self encodeFloatComponent: [:v | v texcoord] count: 2
	]
]

{ #category : #private }
AGPUSampleMesh >> encodedVertexBuffer [
	encodedVertexBuffer ifNotNil: [ ^ encodedVertexBuffer ].
	encodedVertexBufferOffsets := OrderedCollection new.

	encodedVertexBufferOffsets add: 0.
	encodedVertexBuffer := self encodedColors.

	encodedVertexBufferOffsets add: encodedVertexBuffer size.
	encodedVertexBuffer := encodedVertexBuffer , self encodedTangents4.

	encodedVertexBufferOffsets add: encodedVertexBuffer size.
	encodedVertexBuffer := encodedVertexBuffer , self encodedPositions.

	encodedVertexBufferOffsets add: encodedVertexBuffer size.
	encodedVertexBuffer := encodedVertexBuffer , self encodedNormals.

	encodedVertexBufferOffsets add: encodedVertexBuffer size.
	encodedVertexBuffer := encodedVertexBuffer , self encodedTexcoords.

	^ encodedVertexBuffer
]

{ #category : #drawing }
AGPUSampleMesh >> endDrawingUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer [
	vertices ifEmpty: [ ^ self ].
	indices ifEmpty: [ ^ self ].
	
	immediateRenderer endMesh
]

{ #category : #'mesh building' }
AGPUSampleMesh >> finishLastSubmesh [
	submeshes ifNotEmpty: [ 
		submeshes last indexCount: indices size - submeshes last firstIndex
	]
]

{ #category : #accessing }
AGPUSampleMesh >> indexBuffer [
	^ indexBuffer ifNil: [
		indexBuffer := device createBuffer: (AGPUBufferDescription new
			size: self encodedIndices size;
			heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
			usage_modes: AGPU_ELEMENT_ARRAY_BUFFER;
			main_usage_mode: AGPU_ELEMENT_ARRAY_BUFFER;
			stride: 4;
			yourself		
		) initial_data: self encodedIndices
	]
]

{ #category : #initialization }
AGPUSampleMesh >> initialize [
	vertices := OrderedCollection new.
	indices := OrderedCollection new.
	submeshes := OrderedCollection new.
]

{ #category : #accessing }
AGPUSampleMesh >> vertexBinding [
	| buffers offsets |
	vertexBinding ifNotNil: [ ^ vertexBinding ].
	vertexBinding := device createVertexBinding: self vertexLayout.
	
	buffers := AGPU packListOfReferences: ((Array new: 5) atAllPut: self vertexBuffer).
	
	offsets := ByteArray new: encodedVertexBufferOffsets size * 4.
	encodedVertexBufferOffsets doWithIndex: [ :each :index |
		offsets unsignedLongAt: index*4 - 3 put: each
	].
	
	vertexBinding
		bindVertexBuffersWithOffsets: 5 vertex_buffers: buffers offsets: offsets.
	^ vertexBinding
]

{ #category : #accessing }
AGPUSampleMesh >> vertexBuffer [
	^ vertexBuffer ifNil: [
		vertexBuffer := device createBuffer: (AGPUBufferDescription new
			size: self encodedVertexBuffer size;
			heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
			usage_modes: AGPU_ARRAY_BUFFER;
			main_usage_mode: AGPU_ARRAY_BUFFER;
			yourself		
		) initial_data: self encodedVertexBuffer
	]
]

{ #category : #accessing }
AGPUSampleMesh >> vertexLayout [
	| layoutSpec strides encodedStrides |
	vertexLayout ifNotNil: [ ^ vertexLayout ].
	
	layoutSpec := 
		(AGPUVertexAttribDescription new
			binding: AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_COLOR;
			buffer: 0;
			format: AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT;
			offset: 0;
			getHandle) ,
		(AGPUVertexAttribDescription new
			binding: AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TANGENT_4;
			buffer: 1;
			format: AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT;
			offset: 0;
			getHandle) ,
		(AGPUVertexAttribDescription new
			binding: AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_POSITION;
			buffer: 2;
			format: AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT;
			offset: 0;
			getHandle) ,	
		(AGPUVertexAttribDescription new
			binding: AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_NORMAL;
			buffer: 3;
			format: AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT;
			offset: 0;
			getHandle) ,	
		(AGPUVertexAttribDescription new
			binding: AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TEXCOORD;
			buffer: 4;
			format: AGPU_TEXTURE_FORMAT_R32G32_FLOAT;
			offset: 0;
			getHandle).
	strides := #(16 16 12 12 8).
	encodedStrides := ByteArray new: strides size * 4.
	strides doWithIndex: [ :each :index |
		encodedStrides unsignedLongAt: index*4 - 3 put: each
	].
	
	vertexLayout := device createVertexLayout.
	vertexLayout
		addVertexAttributeBindings: strides size vertex_strides: encodedStrides attribute_count: strides size attributes: (AGPUVertexAttribDescription fromHandle: layoutSpec).
	^ vertexLayout
]
