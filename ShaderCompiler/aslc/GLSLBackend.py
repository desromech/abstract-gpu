import StringIO
from HLBackend import *

BasicTypeRawMapping = (
	('void', 'void'),
	('int', 'int'),
	(VectorType.get(BasicType_Int, 2), 'ivec2'),
	(VectorType.get(BasicType_Int, 3), 'ivec3'),
	(VectorType.get(BasicType_Int, 4), 'ivec4'),
	
	('float', 'float'),
	(VectorType.get(BasicType_Float, 2), 'vec2'),
	(VectorType.get(BasicType_Float, 3), 'vec3'),
	(VectorType.get(BasicType_Float, 4), 'vec4'),
	
	('sampler1D', 'sampler1D'),
	('sampler1DArray', 'sampler1DArray'),
	('sampler2D', 'sampler2D'),
	('sampler2DArray', 'sampler2DArray'),
	('sampler2DRect', 'sampler2DRect'),
	('sampler2DRectArray', 'sampler2DRectArray'),
	('samplerCube', 'samplerCube'),
	('samplerCubeArray', 'samplerCubeArray'),
	('sampler3D', 'sampler3D'),
	('sampler3DArray', 'sampler3DArray'),
)

BasicTypeMapping = {}

BinaryOperationMap = {
	BinaryOperation_ADD : '+',
	BinaryOperation_SUB : '-',
	BinaryOperation_MUL : '*',
	BinaryOperation_IDIV : '/',
	BinaryOperation_UDIV : '+',
	BinaryOperation_IREM : '%',
	BinaryOperation_UREM : '%',
	
	BinaryOperation_BITAND :'&',
	BinaryOperation_BITOR : '|',
	BinaryOperation_BITXOR : '^',
	
	BinaryOperation_SHIFTLEFT : '<<',
	BinaryOperation_SHIFTRIGHT : '>>',
	
	BinaryOperation_ILT : '<',
	BinaryOperation_ILE : '<=',
	BinaryOperation_IEQ : '==',
	BinaryOperation_INE : '!=',
	BinaryOperation_IGT : '>',
	BinaryOperation_IGE : '>=',
	
	# Floating point operations
	BinaryOperation_FADD : '+',
	BinaryOperation_FSUB : '-',
	BinaryOperation_FMUL : '*',
	BinaryOperation_FDIV : '/',
	BinaryOperation_FREM : '%',
	
	BinaryOperation_UFLT : '<',
	BinaryOperation_UFLE : '<=',
	BinaryOperation_UFEQ : '==',
	BinaryOperation_UFNE : '!=',
	BinaryOperation_UFGT : '>',
	BinaryOperation_UFGE : '>=',
	
	BinaryOperation_OFLT : 'oflt',
	BinaryOperation_OFLE : 'ofle',
	BinaryOperation_OFEQ : 'ofeq',
	BinaryOperation_OFNE : 'ofne',
	BinaryOperation_OFGT : 'ofgt',
	BinaryOperation_OFGE : 'ofge',
}

for aslName, glslName in BasicTypeRawMapping:
	aslType = BasicTypes.get(aslName, aslName)
	BasicTypeMapping[aslType] = glslName
	
class GLSLCodeGenerator:
	def __init__(self, out):
		self.out = out
		self.indentCount = 0
		
	def write(self, string):
		self.out.write(string)
		
	def writeLine(self, string=""):
		self.write(string)
		self.write("\n")
		
	def writeIndentedLine(self, string):
		for i in range(self.indentCount):
			self.write(' ')
		self.writeLine(string)
		
	def generate(self, shader):
		shader.accept(self)
		
	def beginLevel(self):
		self.indentCount += 2

	def endLevel(self):
		self.indentCount -= 2
		 
	def visitFragmentShader(self, shader):
		self.generateShaderContent(shader)

	def visitVertexShader(self, shader):
		self.generateShaderContent(shader)

	def visitComputeShader(self, shader):
		self.generateShaderContent(shader)

	def visitTessellationControlShader(self, shader):
		self.generateShaderContent(shader)

	def visitTessellationEvaluationShader(self, shader):
		self.generateShaderContent(shader)
		
	def visitFunction(self, function):
		self.writeLine("%s %s(%s) {" % (self.typeToString(function.functionType.returnType), function.name, self.generateFunctionArguments(function)))
		self.beginLevel()
		
		for var in function.variables:
			self.writeIndentedLine("%s %s;" % (self.typeToString(var.type), var.name))

		for stmnt in function.body.statements:
			stmnt.accept(self)
			
		self.endLevel()
		self.writeLine("}")
		
	def visitReturnVoidStatement(self, ret):
		self.writeIndentedLine("return;")

	def visitReturnStatement(self, ret):
		self.writeIndentedLine("return %s;" % ret.value)

	def visitAssignmentStatement(self, assignment):
		self.writeIndentedLine("%s = %s;" % (assignment.reference.accept(self), assignment.value.accept(self)))

	def visitGlobalVariable(self, variable):
		return variable.name

	def visitVariable(self, variable):
		return variable.name

	def visitConstant(self, constant):
		return str(constant.value)

	def visitBinaryExpression(self, expression):
		return "(%s %s %s)" % (expression.left.accept(self), BinaryOperationMap[expression.operation], expression.right.accept(self))  

	def visitIfStatement(self, statement):
		self.writeIndentedLine("if (%s)" % statement.condition.accept(self))
		statement.thenStatement.accept(self)
		if statement.elseStatement is not None:
			self.writeIndentedLine("else")
			statement.elseStatement.accept(self)
			
	def visitBlockStatement(self, block):
		self.writeIndentedLine('{')
		self.beginLevel()
		for statement in block.statements:
			statement.accept(self)
		self.endLevel()
		self.writeIndentedLine('}')

	def visitCallStatement(self, call):
		function = call.function.name
		argStr = ''
		for arg in call.arguments:
			if len(argStr) > 0:
				argStr += ', '
			argStr += arg.accept(self)
		self.writeIndentedLine('%s (%s);' % (function, argStr))
		
	def visitLoopStatement(self, loop):
		if loop.isWhileLoop():
			ifStatement = loop.getFirstStatement()
			cond = ifStatement.condition
			if ifStatement.elseStatement.isBreakStatement():
				self.writeIndentedLine('while(%s)' % cond.accept(self))
				body = ifStatement.thenStatement
			else:
				assert ifStatement.thenStatement.isBreakStatement()
				self.writeIndentedLine('while(!(%s))' % cond.accept(self))
				body = ifStatement.elseStatement
			body.accept(self)
		elif loop.isDoWhileLoop():
			self.writeIndentedLine('do {')
			self.beginLevel()
			ifStatement = loop.getLastStatement()
			for statement in loop.statements:
				if statement is not ifStatement:
					statement.accept(self)
			self.endLevel()
			
			cond = ifStatement.condition
			if ifStatement.elseStatement.isBreakStatement():
				assert ifStatement.thenStatement.isContinueStatement()
				self.writeIndentedLine('} while (%s);' % cond.accept(self))
			else:
				assert ifStatement.thenStatement.isBreakStatement()
				assert ifStatement.elseStatement.isContinueStatement()
				self.writeIndentedLine('} while (!(%s));' % cond.accept(self))
		else:
			self.writeIndentedLine('for(;;) {')
			self.beginLevel()
			for statement in loop.statements:
				statement.accept(self)
			self.endLevel()
			self.writeIndentedLine('}')

	def visitBreakStatement(self, statement):
		self.writeIndentedLine('break;')

	def visitContinueStatement(self, statement):
		self.writeIndentedLine('continue;')
			
	def generateShaderContent(self, shader):
		for structure in shader.structures:
			self.generateStructure(structure)
		if len(shader.structures) > 0:
			self.writeLine()

		for globalVar in shader.globalVariables:
			self.generateGlobalVar(globalVar)
		if len(shader.globalVariables) > 0:
			self.writeLine()
			
		for function in shader.functions:
			function.accept(self)
		
	def generateGlobalVar(self, globalVar):
		self.writeLine("%s %s %s;" % (globalVar.kind, self.typeToString(globalVar.type), globalVar.name))

	def generateStructure(self, structure):
		self.writeLine("struct %s {" % structure.name)
		self.beginLevel()
		for field in structure.fields:
			self.generateStructureField(field)
		self.endLevel()
		self.writeLine("};")
		self.writeLine()

	def generateStructureField(self, field):
		self.writeIndentedLine("%s %s;" % (self.typeToString(field.fieldType), field.name)) 
		
	def generateFunctionArguments(self, function):
		# The main function does not have arguments
		if function.name == "main":
			return ""

		return ""

	def typeToString(self, aslType):
		mapping = BasicTypeMapping.get(aslType, None)
		if mapping is not None:
			return mapping

		if aslType.isReference():
			return self.typeToString(aslType.baseType)
		if aslType.isStructure():
			return aslType.name
		return "UnimplementedType"
		
class GLSLBackend(HighLevelBackend):
	def __init__(self, module):
		HighLevelBackend.__init__(self, module)
		
	def generateShaderCode(self, shader):
		out = StringIO.StringIO()
		generator = GLSLCodeGenerator(out)
		generator.generate(shader)
		print "//", shader.name
		print out.getvalue()
		
	def createMainFunctionNameFor(self, mainFunction):
		return "main"
		
	def mainFunctionArgumentsToGlobal(self):
		return True
		
	def flattenMainStructureArguments(self):
		return True