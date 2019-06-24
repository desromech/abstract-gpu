Class {
	#name : #AGPUExternalUnion,
	#superclass : #AGPUExternalStructure,
	#category : #'AbstractGPU-CoreSqueak'
}

{ #category : #'as yet unclassified' }
AGPUExternalUnion class >> compileFields: specArray withAccessors: aSymbol [
	"Compile a type specification for the FFI machinery.
	Return the newly compiled spec.
	Eventually generate the field accessors according to following rules:
	- aSymbol = #always always generate the accessors
	- aSymbol = #never never generate the accessors
	- aSymbol = #generated only generate the auto-generated accessors
	- aSymbol = #absent only generate the absent accessors"
	| byteSize byteAlignment typeSpec newCompiledSpec |
	(specArray size > 0 and: [specArray first class ~~ Array]) ifTrue:
		[^ self compileAlias: specArray withAccessors: aSymbol].
	byteSize := 0.
	byteAlignment := 1.
	typeSpec := WriteStream on: (WordArray new: 10).
	typeSpec nextPut: FFIFlagStructure.
	"dummy for size"
	specArray do:
		[:spec | | fieldName fieldType isPointerField externalType typeSize typeAlignment selfRefering |
		fieldName := spec first.
		fieldType := spec second.
		isPointerField := fieldType last = $*.
		fieldType := (fieldType findTokens: ' *') first.
		externalType := ExternalType atomicTypeNamed: fieldType.
		selfRefering := isPointerField and: [externalType isNil and: [fieldType = self asString]].
		selfRefering ifTrue: [
			externalType := ExternalType void asPointerType
		] ifFalse:[
			externalType == nil ifTrue: ["non-atomic"
				Symbol
					hasInterned: fieldType
					ifTrue: [:sym | externalType := ExternalType structTypeNamed: sym].
			].
			externalType == nil ifTrue:[
				Transcript show: '(' , fieldType , ' is void)'.
				externalType := ExternalType void.
			].
			isPointerField ifTrue: [externalType := externalType asPointerType: self pointerSize]].
			typeSize := externalType byteSize.
.
			typeAlignment := self alignmentForType: externalType.
			byteAlignment := byteAlignment max: typeAlignment.
			(fieldName notNil and: [self shouldGenerate: fieldName policy: aSymbol]) ifTrue: [
				self defineFieldAccessorsFor: fieldName startingAt: 1 type: externalType.
			].
			typeSpec nextPutAll: (externalType embeddedSpecWithSize: typeSize).
			byteSize := byteSize max: typeSize.
		].
	structureByteAlignment := byteAlignment.
	newCompiledSpec := typeSpec contents.
	newCompiledSpec at: 1 put: ((byteSize alignedTo: byteAlignment) bitOr: FFIFlagStructure).
	^ newCompiledSpec
]
