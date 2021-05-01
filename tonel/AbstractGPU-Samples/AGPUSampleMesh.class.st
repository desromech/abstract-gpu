Class {
	#name : #AGPUSampleMesh,
	#superclass : #Object,
	#instVars : [
		'vertices',
		'submeshes',
		'indices',
		'baseVertex',
		'vertexBuffer',
		'indexBuffer',
		'vertexBinding'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #adding }
AGPUSampleMesh >> addCubeMinX: minX y: minY z: minZ maxX: maxX y: maxY z: maxZ [
	self
		"Left"
		beginTriangles;
		addVertexP: { minX . minY . minZ } N: #(-1.0 0.0 0.0) TC: #(0.0 1.0);
		addVertexP: { minX . maxY . minZ } N: #(-1.0 0.0 0.0) TC: #(1.0 1.0);
		addVertexP: { minX . maxY . maxZ } N: #(-1.0 0.0 0.0) TC: #(1.0 0.0);
		addVertexP: { minX . minY . maxZ } N: #(-1.0 0.0 0.0) TC: #(0.0 0.0);
		addTriangleI1: 1 i2: 0 i3: 2;
		addTriangleI1: 3 i2: 2 i3: 0;

		"Right"
		beginTriangles;
		addVertexP: { maxX . minY . minZ } N: #(1.0 0.0 0.0) TC: #(0.0 1.0);
		addVertexP: { maxX . maxY . minZ } N: #(1.0 0.0 0.0) TC: #(1.0 1.0);
		addVertexP: { maxX . maxY . maxZ } N: #(1.0 0.0 0.0) TC: #(1.0 0.0);
		addVertexP: { maxX . minY . maxZ } N: #(1.0 0.0 0.0) TC: #(0.0 0.0);
		addTriangleI1: 0 i2: 1 i3: 2;
		addTriangleI1: 2 i2: 3 i3: 0;

		"Top"
		beginTriangles;
		addVertexP: { minX . maxY . minZ } N: #(0.0 1.0 0.0) TC: #(0.0 1.0);
		addVertexP: { maxX . maxY . minZ } N: #(0.0 1.0 0.0) TC: #(1.0 1.0);
		addVertexP: { maxX . maxY . maxZ } N: #(0.0 1.0 0.0) TC: #(1.0 0.0);
		addVertexP: { minX . maxY . maxZ } N: #(0.0 1.0 0.0) TC: #(0.0 0.0);
		addTriangleI1: 1 i2: 0 i3: 2;
		addTriangleI1: 3 i2: 2 i3: 0;

		"Bottom"
		beginTriangles;
		addVertexP: { minX . minY . minZ } N: #(0.0 -1.0 0.0) TC: #(0.0 1.0);
		addVertexP: { maxX . minY . minZ } N: #(0.0 -1.0 0.0) TC: #(1.0 1.0);
		addVertexP: { maxX . minY . maxZ } N: #(0.0 -1.0 0.0) TC: #(1.0 0.0);
		addVertexP: { minX . minY . maxZ } N: #(0.0 -1.0 0.0) TC: #(0.0 0.0);
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
		normal: normal;
		texcoord: texcoord;
		tangent4: tangent4;
		yourself)
]

{ #category : #adding }
AGPUSampleMesh >> addVertexP: position N: normal TC: texcoord [
	vertices add: (AGPUSampleMeshVertex new
		position: position;
		normal: normal;
		texcoord: texcoord;
		tangent4: #(1 0 0 1);
		yourself)
]

{ #category : #'mesh building' }
AGPUSampleMesh >> beginPrimitives: primitiveTopology [
	baseVertex := vertices size.
	(submeshes isNotEmpty and: [ submeshes last primitiveTopology == primitiveTopology ]) ifTrue: [ ^ self ].
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

{ #category : #'as yet unclassified' }
AGPUSampleMesh >> drawWithImmediateRenderer: immediateRenderer [
	submeshes do: [ :each |
		each drawVertices: vertices indices: indices withImmediateRenderer: immediateRenderer
	]
]

{ #category : #'as yet unclassified' }
AGPUSampleMesh >> finishLastSubmesh [
	submeshes ifNotEmpty: [ 
		submeshes last indexCount: indices size - submeshes last firstIndex
	]
]

{ #category : #initialization }
AGPUSampleMesh >> initialize [
	vertices := OrderedCollection new.
	indices := OrderedCollection new.
	submeshes := OrderedCollection new.
]
