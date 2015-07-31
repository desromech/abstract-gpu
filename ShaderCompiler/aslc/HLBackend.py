from Type import *
from SSA import *

GLOBAL_KIND_UNIFORM = 'uniform' 
class HLBNode:
	def __init__(self):
		pass

	def isFlattenedStructure(self):
		return False

	def isIfStatement(self):
		return False

	def isBreakStatement(self):
		return False
		
	def isContinueStatement(self):
		return False
					
class HLBExpression(HLBNode):
	def __init__(self):
		pass
				
class HLBFunction(HLBExpression):
	def __init__(self, name, functionType):
		self.name = name
		self.arguments = []
		self.variables = []
		self.functionType = functionType
		self.body = None

	def accept(self, visitor):
		return visitor.visitFunction(self)
		
class HLBFunctionArgument(HLBExpression):
	def __init__(self):
		pass

	def accept(self, visitor):
		return visitor.visitFunctionArgument(self)

class HLBGlobalVariable(HLBExpression):
	def __init__(self, name, type, kind='normal'):
		self.name = name
		self.type = type
		self.kind = kind

	def accept(self, visitor):
		return visitor.visitGlobalVariable(self)

class HLBVariable(HLBExpression):
	def __init__(self, name, type):
		self.name = name
		self.type = type
		
	def accept(self, visitor):
		return visitor.visitVariable(self)

class HLBFlattenedStructure(HLBNode):
	def __init__(self):
		self.fields = []
		
	def isFlattenedStructure(self):
		return True
				
class HLBShader(HLBNode):
	def __init__(self, name):
		self.name = name
		self.globalVariables = []
		self.functions = []
		self.structures = []
		
class HLBFragmentShader(HLBShader):
	def accept(self, visitor):
		return visitor.visitFragmentShader(self)
	
class HLBVertexShader(HLBShader):
	def accept(self, visitor):
		return visitor.visitVertexShader(self)

class HLBGeometryShader(HLBShader):
	def accept(self, visitor):
		return visitor.visitGeometryShader(self)
	
class HLBComputeShader(HLBShader):
	def accept(self, visitor):
		return visitor.visitComputeShader(self)

class HLBTessellationControlShader(HLBShader):
	def accept(self, visitor):
		return visitor.visitTessellationControlShader(self)

class HLBTessellationEvaluationShader(HLBShader):
	def accept(self, visitor):
		return visitor.visitTessellationEvaluationShader(self)
	
class HLBStatement(HLBNode):
	def __init__(self):
		pass

class HLBReturnStatement(HLBStatement):
	def __init__(self):
		pass

	def accept(self, visitor):
		return visitor.visitReturnStatement(self)

class HLBBreakStatement(HLBStatement):
	def __init__(self):
		pass

	def accept(self, visitor):
		return visitor.visitBreakStatement(self)
		
	def isBreakStatement(self):
		return True

class HLBContinueStatement(HLBStatement):
	def __init__(self):
		pass

	def accept(self, visitor):
		return visitor.visitContinueStatement(self)

	def isContinueStatement(self):
		return True
		
class HLBAssignmentStatement(HLBStatement):
	def __init__(self, reference, value):
		self.reference = reference
		self.value = value
		
	def accept(self, visitor):
		return visitor.visitAssignmentStatement(self)
		
class HLBReturnVoidStatement(HLBStatement):
	def __init__(self):
		pass

	def accept(self, visitor):
		return visitor.visitReturnVoidStatement(self)

class HLBBlockStatement(HLBStatement):
	def __init__(self):
		self.statements = []

	def accept(self, visitor):
		return visitor.visitBlockStatement(self)
		
	def addStatement(self, statement):
		self.statements.append(statement)
		
	def getFirstStatement(self):
		return self.statements[0]

class HLBLoopStatement(HLBBlockStatement):
	def accept(self, visitor):
		return visitor.visitLoopStatement(self)
		
	def isWhileLoop(self):
		if len(self.statements) == 1:
			statement = self.statements[0]
			if statement.isIfStatement():
				thenStmnt = statement.thenStatement
				elseStmnt = statement.elseStatement
				return (elseStmnt is not None and elseStmnt.isBreakStatement()) or (thenStmnt is not None and thenStmnt.isBreakStatement())
		return False

	def isDoWhileLoop(self):
		if len(self.statements) > 0:
			statement = self.statements[-1]
			if statement.isIfStatement():
				return (statement.thenStatement.isContinueStatement() and statement.elseStatement.isBreakStatement()) or \
					(statement.thenStatement.isBreakStatement() and statement.elseStatement.isContinueStatement())
		return False

class HLBIfStatement(HLBStatement):
	def __init__(self, condition, thenStatement, elseStatement):
		self.condition = condition
		self.thenStatement = thenStatement
		self.elseStatement = elseStatement

	def accept(self, visitor):
		return visitor.visitIfStatement(self)
		
	def isIfStatement(self):
		return True

class HLBConstant(HLBExpression):
	def __init__(self, value):
		self.value = value

	def accept(self, visitor):
		return visitor.visitConstant(self)

class HLBCallExpression(HLBNode):
	def __init__(self, function, arguments):
		self.function = function
		self.arguments = arguments

	def accept(self, visitor):
		return visitor.visitCallExpression(self)

class HLBBinaryExpression(HLBNode):
	def __init__(self, operation, left, right):
		self.operation = operation
		self.left = left
		self.right = right

	def accept(self, visitor):
		return visitor.visitBinaryExpression(self)

class HLBUnaryExpression(HLBNode):
	def __init__(self, operation, operand):
		self.operation = operation
		self.operand = operand
		
	def accept(self, visitor):
		return visitor.visitUnaryExpression(self)

class HLBTernaryExpression(HLBNode):
	def __init__(self):
		pass
		
		
ShaderKindMap = {
	FUNCTION_KIND_VERTEX : HLBVertexShader,
	FUNCTION_KIND_FRAGMENT : HLBFragmentShader,
	FUNCTION_KIND_GEOMETRY : HLBGeometryShader,
	FUNCTION_KIND_TESSELLATION_CONTROL : HLBTessellationControlShader,
	FUNCTION_KIND_TESSELLATION_EVALUATION : HLBTessellationEvaluationShader,
	FUNCTION_KIND_COMPUTE : HLBComputeShader,
}

class HLBFunctionBuilder:
	def __init__(self, context, function, hlbFunction, argumentsAsGlobals = False):
		self.context = context
		self.function = function
		self.hlbFunction = hlbFunction
		self.generatedVarNameCount = 0
		self.generatedBlocks = {}
		self.mergeBlocks = set()
		self.valueMap = {}
		self.currentBlock = HLBBlockStatement()
		self.currentBreakBlock = None
		self.currentContinueBlock = None
		self.argumentsAsGlobals = argumentsAsGlobals
		
	def addStatement(self, statement):
		self.currentBlock.addStatement(statement)

	def addBreak(self):
		self.currentBlock.addStatement(HLBBreakStatement())

	def addContinue(self):
		self.currentBlock.addStatement(HLBContinueStatement())
		
	def addVariable(self, valueType, name):
		self.addType(valueType)
		variable = HLBVariable(name, valueType)
		self.hlbFunction.variables.append(variable)
		return variable
		
	def addType(self, type):
		self.context.addType(type)
		
	def addInstructionMapping(self, instruction, mapping):
		self.valueMap[instruction] = mapping
		
	def mapInstruction(self, instruction):
		result = self.valueMap.get(instruction, None)
		if result is not None:
			return result

		if instruction.isConstant():
			return self.addConstant(instruction)
		self.error('Failed to map ssa instruction into code.')

	def addConstant(self, instruction):
		if instruction.isGlobalValue():
			if instruction.isFunction():
				return self.context.addFunction(instruction)
			self.error("Not supported yet")
		
		constant = HLBConstant(instruction.getValue())
		return constant
		
	def generateVariableName(self):
		self.generatedVarNameCount += 1
		return "gv" + str(self.generatedVarNameCount)

	def build(self):
		print self.function
		self.function.findLoops()
		self.generateArguments()
		self.hlbFunction.body = self.generateBasicBlock(self.function.getEntryPoint())
		
	def generateArguments(self):
		if self.argumentsAsGlobals:
			self.moveArgumentsToGlobals()
		else:
			self.hlbFunction.arguments = list(map(self.generateArgument, self.function.arguments))

	def moveArgumentsToGlobals(self):
		for arg in self.function.arguments:
			self.addInstructionMapping(arg, self.context.argumentToGlobal(arg))
		
	def generateArgument(self, argument):
		pass
		
	def generateBasicBlock(self, basicBlock):
		# Only generate the blocks once
		oldBlock = self.generatedBlocks.get(basicBlock, None)
		if oldBlock is not None:
			return None
		oldBlock = self.currentBlock
		self.generatedBlocks[basicBlock] = oldBlock
		self.currentMergeBlock = basicBlock.mergeBlock
		
		# Loop state
		oldBreakBlock = self.currentBreakBlock
		oldContinueBlock = self.currentContinueBlock
		breakBlock = None
		
		# Loop beginning
		loop = basicBlock.loop
		addedMerge = False
		if loop is not None and loop.header is basicBlock:
			header = loop.header
			breakBlock = basicBlock.mergeBlock
			newBlock = HLBLoopStatement()
			self.addStatement(newBlock)
			self.currentBlock = newBlock
			self.currentBreakBlock = breakBlock
			self.currentContinueBlock = header
			if breakBlock is not None and breakBlock not in self.mergeBlocks:
				addedMerge = True
				self.mergeBlocks.add(breakBlock)
				
		# Ensure we are emitting into a block
		block = self.currentBlock
		self.currentBasicBlock = basicBlock
		
		# Generate each one of the instructions.
		for instruction in basicBlock.instructions:
			instruction.accept(self)

		# Restore the block state
		self.currentBreakBlock = oldBreakBlock
		self.currentContinueBlock = oldContinueBlock 
		self.currentBlock = oldBlock
		 
		# Generate after the loop
		if breakBlock is not None and addedMerge:
			self.generateBasicBlock(breakBlock)
		return oldBlock
			
	def visitJumpInstruction(self, instruction):
		targetBlock = instruction.getTargetBlock()
		if targetBlock is self.currentBreakBlock:
			self.addBreak()
		elif targetBlock is self.currentContinueBlock:
			self.addContinue()
		elif targetBlock not in self.mergeBlocks:
			self.generateBasicBlock(targetBlock)
				
	def generateJumpBlock(self, targetBlock, optional=False):
		if targetBlock is self.currentBreakBlock:
			return HLBBreakStatement()
		elif targetBlock is self.currentContinueBlock:
			return HLBContinueStatement()
		elif targetBlock not in self.mergeBlocks:
			return self.generateBasicBlock(targetBlock)
		else:
			assert optional
			return None

	def visitBranchInstruction(self, instruction):
		ifBlock = self.currentBlock
		condition = self.mapInstruction(instruction.getCondition())
		thenBlock = instruction.getThenBlock()
		elseBlock = instruction.getElseBlock()
		
		continueBlock = self.currentMergeBlock.mergeBlock
		if continueBlock not in self.mergeBlocks:
			addedMerge = True
			self.mergeBlocks.add(continueBlock)
			
		if elseBlock is continueBlock:
			self.currentBlock = HLBBlockStatement()
			thenStatement = self.generateJumpBlock(thenBlock)
			statement = HLBIfStatement(condition, thenStatement, None)
		else:
			self.currentBlock = HLBBlockStatement()
			thenStatement = self.generateJumpBlock(thenBlock)
			self.currentBlock = HLBBlockStatement()
			elseStatement = self.generateJumpBlock(elseBlock, True)
			statement = HLBIfStatement(condition, thenStatement, elseStatement)

		# Restore the block.
		self.currentBlock = ifBlock
		self.addStatement(statement)
		
		# Merge the if branches.
		if continueBlock is not None and addedMerge:
			self.mergeBlocks.remove(continueBlock)
			self.generateJumpBlock(continueBlock)

	def visitReturnInstruction(self, instruction):
		pass

	def visitReturnVoidInstruction(self, instruction):
		self.addStatement(HLBReturnVoidStatement())

	def visitAllocaInstruction(self, instruction):
		valueType = instruction.getValueType()
		name = self.generateVariableName()
		var = self.addVariable(valueType, name)
		self.addInstructionMapping(instruction, var)

	def visitLoadInstruction(self, instruction):
		reference = self.mapInstruction(instruction.getReference())
		self.addInstructionMapping(instruction, reference)

	def visitStoreInstruction(self, instruction):
		reference = self.mapInstruction(instruction.getReference())
		value = self.mapInstruction(instruction.getValue())
		self.addStatement(HLBAssignmentStatement(reference, value))
		
	def visitCallInstruction(self, instruction):
		function = self.mapInstruction(instruction.getFunction())
		arguments = list(map(self.mapInstruction, instruction.getArguments()))
		expression = HLBCallExpression(function, arguments)
		
	def visitBinaryOperationInstruction(self, instruction):
		left = self.mapInstruction(instruction.getLeft())
		right = self.mapInstruction(instruction.getRight())
		self.addInstructionMapping(instruction, HLBBinaryExpression(instruction.operation, left, right))

	def visitGetElementReferenceInstruction(self, instruction):
		baseReference = self.mapInstruction(instruction.getBaseReference())
		for i in instruction.indirections:
			if baseReference.isFlattenedStructure():
				baseReference = baseReference.fields[i]
			else:
				baseType = baseReference.getType()
				self.unimplemented()
				
		self.addInstructionMapping(instruction, baseReference)
		
class HLBShaderBuilder:
	def __init__(self, module, shader, context):
		self.module = module
		self.shader = shader
		self.generatedFunctionNameCount = 0
		self.context = context
		self.functions = {}
		
	def addType(self, type):
		if type.isReference():
			return self.addType(type.baseType)
			
		if type.isStructure():
			self.addStructureType(type)

	def addStructureType(self, structure):
		for f in structure.fields:
			self.addType(f.fieldType)
		self.shader.structures.append(structure)
		
	def addFunction(self, function):
		hlbFunction = self.functions.get(function, None)
		if hlbFunction is not None:
			return hlbFunction
			
		return self.buildFunction(function)
		
	def generateFunctionName(self):
		self.generatedFunctionNameCount += 1
		return "gf" + str(self.generatedFunctionNameCount)

	def buildFunction(self, function, functionName = None, moveArgumentsToGlobal = False):
		if functionName is None:
			functionName = self.generateFunctionName()
			
		hlbFunction = HLBFunction(functionName, function.getType())
		self.functions[function] = hlbFunction
		self.shader.functions.append(hlbFunction)
		
		# Build the function content
		functionBuilder = HLBFunctionBuilder(self, function, hlbFunction, moveArgumentsToGlobal)
		functionBuilder.build()
		return hlbFunction

	def buildMainFunction(self, mainFunction, mainFunctionName):
		self.shader.mainFunction = self.buildFunction(mainFunction, mainFunctionName, self.context.mainFunctionArgumentsToGlobal())
		
	def argumentToGlobal(self, argument):
		varName = argument.name;
		varType = argument.getType();

		# The argument type should be always a reference
		assert varType.isReference()
		varType = varType.baseType
		kind = argument.kind
		
		# Kind can only be in or out.
		if kind not in ('in', 'out', 'uniform'):
			self.error("Cannot move shader main function argument into global space.")
			
		# If flattening the main structure argument, take some special cares.
		if self.context.flattenMainStructureArguments() and varType.isStructure():
			return self.flattenStructureArgument(varName + '_', varType, kind)
			
		return self.addGlobal(varName, varType, kind)
		
	def flattenStructureArgument(self, prefix, structureType, kind):
		flatStruct = HLBFlattenedStructure()
		for field in structureType.fields:
			fieldName = field.name
			fieldType = field.fieldType
			fullName = prefix + fieldName
			if fieldType.isStructure():
				flatStruct.fields.append(flattenStructureArgument(fullName + '_', fieldType, kind))
			else:
				flatStruct.fields.append(self.addGlobal(fullName, fieldType, kind, field.semantics))
		return flatStruct
		
	def addGlobal(self, name, type, kind, semantics = {}):
		self.addType(type)
		globalVar = HLBGlobalVariable(name, type, kind)
		self.shader.globalVariables.append(globalVar)
		return globalVar
		
		
# High level backend
class HighLevelBackend:
	def __init__(self, module):
		self.module = module
		self.shaders = []

	def buildModuleRepresentation(self):
		for function in self.module.getFunctions():
			if function.getType().kind != FUNCTION_KIND_NORMAL:
				self.buildShaderRepresentation(function)

	def buildShaderRepresentation(self, mainFunction):
		shaderClass = ShaderKindMap[mainFunction.getType().kind]
		shader = shaderClass(mainFunction.name)
		
		builder = HLBShaderBuilder(self.module, shader, self)
		builder.buildMainFunction(mainFunction, self.createMainFunctionNameFor(mainFunction))
		self.shaders.append(shader)
		
	def generate(self, outputPath):
		self.buildModuleRepresentation()
		self.outputPath = outputPath
		for shader in self.shaders:
			self.generateShaderCode(shader)
		
	def generateShaderCode(self, shader):
		# Subclass responsibility
		pass

	def mainFunctionArgumentsToGlobal(self):
		return False
		
	def flattenMainStructureArguments(self):
		return False
		