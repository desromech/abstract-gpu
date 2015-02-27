from Type import *

# Integer operations
BinaryOperation_ADD = 'add'
BinaryOperation_SUB = 'sub'
BinaryOperation_MUL = 'mul'
BinaryOperation_IDIV = 'div'
BinaryOperation_UDIV = 'udiv'
BinaryOperation_IREM = 'rem'
BinaryOperation_UREM = 'urem'

BinaryOperation_BITAND = 'bitand'
BinaryOperation_BITOR = 'bitor'
BinaryOperation_BITXOR = 'bitxor'

BinaryOperation_SHIFTLEFT = 'shiftleft'
BinaryOperation_SHIFTRIGHT = 'shiftright'

BinaryOperation_ILT = 'ilt'
BinaryOperation_ILE = 'ile'
BinaryOperation_IEQ = 'ieq'
BinaryOperation_INE = 'ine'
BinaryOperation_IGT = 'igt'
BinaryOperation_IGE = 'ige'

# Floating point operations
BinaryOperation_FADD = 'fadd'
BinaryOperation_FSUB = 'fsub'
BinaryOperation_FMUL = 'fmul'
BinaryOperation_FDIV = 'fdiv'
BinaryOperation_FREM = 'frem'

BinaryOperation_UFLT = 'uflt'
BinaryOperation_UFLE = 'ufle'
BinaryOperation_UFEQ = 'ufeq'
BinaryOperation_UFNE = 'ufne'
BinaryOperation_UFGT = 'ufgt'
BinaryOperation_UFGE = 'ufge'

BinaryOperation_OFLT = 'oflt'
BinaryOperation_OFLE = 'ofle'
BinaryOperation_OFEQ = 'ofeq'
BinaryOperation_OFNE = 'ofne'
BinaryOperation_OFGT = 'ofgt'
BinaryOperation_OFGE = 'ofge'

ComparisonOperations = set((
    BinaryOperation_ILT,
    BinaryOperation_ILE,
    BinaryOperation_IEQ,
    BinaryOperation_INE,
    BinaryOperation_IGT,
    BinaryOperation_IGE,

    BinaryOperation_UFLT,
    BinaryOperation_UFLE,
    BinaryOperation_UFEQ,
    BinaryOperation_UFNE,
    BinaryOperation_UFGT,
    BinaryOperation_UFGE,

    BinaryOperation_OFLT,
    BinaryOperation_OFLE,
    BinaryOperation_OFEQ,
    BinaryOperation_OFNE,
    BinaryOperation_OFGT,
    BinaryOperation_OFGE,
))

class Module:
    def __init__(self):
        self.symbols = {}

    def addGlobalValue(self, value):
        self.symbols[value.getName()] = value

    def __str__(self):
        result = '# Module\n'
        for symbol in self.symbols.values():
            result += '%s\n' % str(symbol)
        return result

class Value:
    def __init__(self):
        pass

    def getType(self):
        raise Exception(self.__class__.__name__)

    def isType(self):
        return False
        
    def isConstant(self):
        return False

    def isTerminator(self):
        return False

    def addInstructionReference(self, reference):
        pass

    def replaceParameterWith(self, param, newParam):
        pass

class Constant(Value):
    def __init__(self):
        pass

    def isConstant(self):
        return True

# Constant values
class ConstantValue(Constant):
    constantValues = {}
    def __init__(self, tpe, value):
        assert (tpe, value) not in self.constantValues
        self.constantValues[(tpe, value)] = self

        self.type = tpe
        self.value = value

    @classmethod
    def get(cls, tpe, value):
        constant = cls.constantValues.get((tpe, value), None)
        if constant is not None:
            return constant
        else:
            return ConstantValue(tpe, value)
        
    def getType(self):
        return self.type

    def getValue(self):
        return self.value

    def __str__(self):
        return 'constant [%s]%s' % (str(self.type), str(self.value))
            
class GlobalValue(Constant):
    def __init__(self, name, tpe):
        self.name = name
        self.type = tpe

    def getName(self):
        return self.name

    def getType(self):
        return self.type

class GlobalVariable(GlobalValue):
    def __init__(self, tpe):
        GlobalValue.__init__(tpe)

    def __str__(self):
        return '%s $%s = %s;\n' % (str(self.type), self.name, str(self.value))

class Function(GlobalValue):
    def __init__(self, name, tpe):
        GlobalValue.__init__(self, name, tpe)
        self.makeArguments()
        self.basicBlocks = []
        self.gensymCount = 1

    def isDeclaration(self):
        return len(self.basicBlocks) == 0

    def __str__(self):
        if self.isDeclaration():
           return 'declare $"%s" %s;\n' % (self.name, str(self.type))
        result = 'define $"%s" %s {\n'  % (self.name, str(self.type))
        for basicBlock in self.basicBlocks:
            result += str(basicBlock)
        result += '}\n'

        return result

    def makeArguments(self):
        self.arguments = []
        for i in range(len(self.type.arguments)):
            argType = self.type.arguments[i]
            self.arguments.append(FunctionArgument(i, argType))

    def addBasicBlock(self, basicBlock):
        self.basicBlocks.append(basicBlock)

    def generateSymbol(self):
        result = 'g%d' % self.gensymCount
        self.gensymCount += 1
        return result

class FunctionArgument(Value):
    def __init__(self, index, argumentType):
        self.name = None
        self.index = index
        self.kind = argumentType.kind
        self.type = argumentType.type

    def getType(self):
        return self.type

    def __str__(self):
        if self.name is not None:
            return '$%s' % self.name
        else:
            return '$%d' % self.index

class BasicBlock:
    def __init__(self, function, name=None):
        self.name = name
        self.function = function
        self.instructions = []
        function.addBasicBlock(self)

        if self.name is None:
            self.name = function.generateSymbol()

    def addInstruction(self, instruction):
        self.instructions.append(instruction)

    def getLastInstruction(self):
        if len(self.instructions) == 0:
            return None
        return self.instructions[-1]

    def __str__(self):
        result = '%s:\n'  % self.name
        for instruction in self.instructions:
            result += '  %s\n' % instruction.lineString()
        return result

    def replacedInstructionWith(self, oldInstruction, newInstruction):
        for i in range(len(self.instructions)):
            if self.instructions[i] == oldInstruction:
                # Only replace when the new instruction is an instruction
                if newInstruction.isInstruction():
                    self.instructions[i] = newInstruction
                else:
                    del self.instructions[i]
        
class BlockBuilder:
    def __init__(self, basicBlock):
        self.function = basicBlock.function
        self.currentBlock = basicBlock

    def addInstruction(self, instruction, name=None):
        if name is None:
            name = self.function.generateSymbol()
        instruction.name = name

        instruction.ownerBlock = self.currentBlock
        self.currentBlock.addInstruction(instruction)
        return instruction

    def addBinaryOperation(self, operation, left, right, name=None):
        return self.addInstruction(BinaryOperationInstruction(operation, left, right), name)

    def addComparison(self, operation, left, right, name=None):
        return self.addInstruction(ComparisonInstruction(operation, left, right), name)

    def getLastInstruction(self):
        return self.currentBlock.getLastInstruction()

    def isLastTerminator(self):
        last = self.getLastInstruction()
        return last is not None and last.isTerminator()

    def setInsertBlock(self, block):
        self.currentBlock = block

    def alloca(self, valueType, name=None):
        return self.addInstruction(AllocaInstruction(valueType), name)

    def load(self, reference, name=None):
        return self.addInstruction(LoadInstruction(reference), name)

    def store(self, value, reference, name=None):
        return self.addInstruction(StoreInstruction(value, reference), name)

    def branch(self, condition, thenBlock, elseBlock, name=None):
        return self.addInstruction(BranchInstruction(condition, thenBlock, elseBlock), name=None)

    def jump(self, targetBlock, name=None):
        return self.addInstruction(JumpInstruction(targetBlock), name=None)

    def unreachable(self, name=None):
        return self.addInstruction(UnreachableInstruction(), name=None)

    def returnValue(self, value, name=None):
        return self.addInstruction(ReturnInstruction(value), name=None)

    def returnVoid(self, name=None):
        return self.addInstruction(ReturnVoidInstruction(), name=None)

    def add(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_ADD, left, right, name)

    def sub(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_SUB, left, right, name)

    def mul(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_MUL, left, right, name)

    def div(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_DIV, left, right, name)

    def rem(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_REM, left, right, name)

    def fadd(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_FADD, left, right, name)

    def fsub(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_FSUB, left, right, name)

    def fmul(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_FMUL, left, right, name)

    def fdiv(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_FDIV, left, right, name)

    def frem(self, left, right, name=None):
        return self.addBinaryOperation(BinaryOperation_FREM, left, right, name)

class Instruction(Value):
    def __init__(self, parameters):
        self.ownerBlock = None
        self.name = ''
        self.parameters = parameters
        self.instructionReferences = []

        for param in parameters:
            param.addInstructionReference(self)

    def addInstructionReference(self, reference):
        self.instructionReferences.append(reference)

    def replaceParameterWith(self, param, newParam):
        for i in range(len(self.parameters)):
            if self.parameters[i] == param:
                self.parameters[i] = newParam

    def replaceWith(self, other):
        self.ownerBlock.replacedInstructionWith(self, other)
        for ref in instructionReferences:
            ref.replaceParameterWith(self, other)

    def definitionString(self):
        pass

    def lineString(self):
        tpe = self.getType()
        if tpe is not None:
            return '[%s] $%s = %s' % (str(self.getType()), self.name, self.definitionString())
        else:
            return self.definitionString()

    def __str__(self):
        return '$%s' % self.name

class AllocaInstruction(Instruction):
    def __init__(self, valueType):
        Instruction.__init__(self, [])
        self.valueType = valueType
        self.type = ReferenceType.get(valueType)

    def getValueType(self):
        return self.valueType

    def getType(self):
        return self.type

    def definitionString(self):
        return 'alloca %s' % str(self.valueType)

class LoadInstruction(Instruction):
    def __init__(self, reference):
        Instruction.__init__(self, [reference])

        assert reference.getType().isReference()
        self.type = reference.getType().baseType

    def getType(self):
        return self.type

    def getReference(self):
        return self.parameters[0]

    def definitionString(self):
        return 'load from %s' % str(self.getReference())

class StoreInstruction(Instruction):
    def __init__(self, value, reference):
        Instruction.__init__(self, [value, reference])

    def getType(self):
        return None

    def getReference(self):
        return self.parameters[1]

    def getValue(self):
        return self.parameters[0]

    def definitionString(self):
        return 'store %s in %s' % (str(self.getValue()), str(self.getReference()))

class TerminatorInstruction(Instruction):
    def getType(self):
        return None
        
    def isTerminator(self):
        return True

class JumpInstruction(TerminatorInstruction):
    def __init__(self, targetBlock):
        TerminatorInstruction.__init__(self, [])
        self.targetBlock = targetBlock

    def getTargetBlock(self, targetBlock):
        return self.targetBlock

    def definitionString(self):
        return 'jump @%s' % self.targetBlock.name

class BranchInstruction(TerminatorInstruction):
    def __init__(self, condition, thenBlock, elseBlock):
        TerminatorInstruction.__init__(self, [condition])
        self.thenBlock = thenBlock
        self.elseBlock = elseBlock

        assert condition.getType().isBoolean()

    def getCondition(self):
        return self.parameters[0]

    def getTargetBlock(self, targetBlock):
        return self.targetBlock

    def definitionString(self):
        return 'branch %s then @%s else @%s' % (str(self.getCondition()), self.thenBlock.name, self.elseBlock.name)

class UnaryOperationInstruction(Instruction):
    def __init__(self, operation, operand):
        Instruction.__init__(self, [operand])
        self.operation = operation

    def getOperand(self):
        return parameters[0]

    def definitionString(self):
        return '%s %s %s' % (self.operation, str(self.getOperand()))

class BinaryOperationInstruction(Instruction):

    def __init__(self, operation, left, right):
        Instruction.__init__(self, [left, right])
        self.operation = operation
        self.type = self.computeType(self.operation, left.getType(), right.getType())

    def getLeft(self):
        return self.parameters[0]

    def getRight(self):
        return self.parameters[1]

    def getType(self):
        return self.type

    def definitionString(self):
        return '%s %s %s' % (self.operation, str(self.getLeft()), str(self.getRight()))
        
    def computeType(self, operation, leftType, rightType):
        assert leftType == rightType
        if operation in ComparisonOperations:
            return BasicType_Bool
        return leftType
        
class UnreachableInstruction(TerminatorInstruction):
    def __init__(self):
        TerminatorInstruction.__init__(self, [])

    def definitionString(self):
        return 'unreachable'

class ReturnVoidInstruction(TerminatorInstruction):
    def __init__(self):
        TerminatorInstruction.__init__(self, [])

    def definitionString(self):
        return 'return void'

class ReturnInstruction(TerminatorInstruction):
    def __init__(self, value):
        TerminatorInstruction.__init__(self, [value])

    def getValue(self):
        return self.parameters[0]

    def definitionString(self):
        return 'return %s' % str(self.getValue())


