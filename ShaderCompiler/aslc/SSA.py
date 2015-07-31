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

    def getFunctions(self):
        return filter(lambda gv: gv.isFunction(), self.symbols.values())
        
class Value:
    def __init__(self):
        pass

    def getType(self):
        raise Exception(self.__class__.__name__)

    def isType(self):
        return False
        
    def isConstant(self):
        return False
        
    def isGlobalValue(self):
        return False

    def isInstruction(self):
        return False

    def isTerminator(self):
        return False

    def isExit(self):
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
        
    def isGlobalValue(self):
        return True
        
    def isFunction(self):
        return False

    def isGlobalVariable(self):
        return False

class GlobalVariable(GlobalValue):
    def __init__(self, tpe):
        GlobalValue.__init__(tpe)

    def __str__(self):
        return '%s $%s = %s;\n' % (str(self.type), self.name, str(self.value))

    def isGlobalVariable(self):
        return True

class ControlFlowLoop:
    def __init__(self):
        self.header = None
        self.body = set()
        self.parentLoop = None

    def findStartingAtBlock(self, block):
        self.header = block
        self.parentLoop = block.loop
        
        for pred in block.getPredecessors():
            # Find the back edges in the predecessor
            if block.dominates(pred):
                self.body.add(block)
                block.loop = self
                self.addLoopBody(pred)
                
        # Is this a loop?
        if len(self.body) == 0:
            return False
            
        return True        
                
    def addLoopBody(self, node):
        if node not in self.body:
            self.body.add(node)
            node.loop = self
            for pred in node.getPredecessors():
                self.addLoopBody(pred)
                
class Function(GlobalValue):
    def __init__(self, name, tpe):
        GlobalValue.__init__(self, name, tpe)
        self.makeArguments()
        self.basicBlocks = []
        self.gensymCount = 1
        self.haveDominance = False
        self.havePostDominance = False
        self.loops = None
        self.exitBlocks = None

    def getExitBlocks(self):
        if self.exitBlocks is not None:
            return self.exitBlocks
            
        self.exitBlocks = []
        for block in self.basicBlocks:
            if block.isExitBlock():
                self.exitBlocks.append(block)
        return self.exitBlocks
        
    def isFunction(self):
        return True

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
        basicBlock.index = len(self.basicBlocks)
        self.basicBlocks.append(basicBlock)
        self.postOrderCache = None
        self.exitPostOrderCache = None
        self.preOrderCache = None  
        self.haveDominance = False
        self.havePostDominance = False
        self.loops = None

    def generateSymbol(self):
        result = 'g%d' % self.gensymCount
        self.gensymCount += 1
        return result

    def getEntryPoint(self):
        return self.basicBlocks[0]

    def getExitPostOrder(self):
        # Return the cached exit post order
        if self.exitPostOrderCache is not None:
            return self.exitPostOrderCache

        # Clear the post order cache
        self.exitPostOrderCache = []
        
        # Reset the visited flag
        visited = [False] * len(self.basicBlocks)

        # Do the dfs.
        def postOrder(node):
            assert node.index >= 0
            if visited[node.index]:
                return

            visited[node.index] = True
            for pred in node.getPredecessors():
                postOrder(pred)
            self.exitPostOrderCache.append(node)

        # Build the post order
        blocks = self.getExitBlocks()
        for exit in blocks:
            postOrder(exit)

        # Store the post order index in the nodes
        for i in range(len(self.exitPostOrderCache)):
            self.exitPostOrderCache[i].exitPostOrderIndex = i

        return self.exitPostOrderCache
        
    def getPostOrder(self):
        # Return the cached post order
        if self.postOrderCache is not None:
            return self.postOrderCache

        # Clear the post order cache
        self.postOrderCache = []
        
        # Reset the visited flag
        visited = [False] * len(self.basicBlocks)

        # Do the dfs.
        def postOrder(node):
            assert node.index >= 0
            if visited[node.index]:
                return

            visited[node.index] = True
            for successor in node.getSuccessors():
                postOrder(successor)
            self.postOrderCache.append(node)

        # Build the post order
        if len(self.basicBlocks) != 0:
            postOrder(self.basicBlocks[0])

        # Store the post order index in the nodes
        for i in range(len(self.postOrderCache)):
            self.postOrderCache[i].postOrderIndex = i

        return self.postOrderCache
        
    def getPreOrder(self):
        # Return the cached post order
        if self.preOrderCache is not None:
            return self.preOrderCache

        # Clear the post order cache
        self.preOrderCache = []
        
        # Reset the visited flag
        visited = [False] * len(self.basicBlocks)

        # Do the dfs.
        def preOrder(node):
            assert node.index >= 0
            if visited[node.index]:
                return

            visited[node.index] = True
            self.preOrderCache.append(node)
            for successor in node.getSuccessors():
                preOrder(successor)

        # Build the pre order
        if len(self.basicBlocks) != 0:
            preOrder(self.basicBlocks[0])

        # Store pre post order index in the nodes
        for i in range(len(self.preOrderCache)):
            self.preOrderCache[i].preOrderIndex = i

        return self.preOrderCache

    #
    # Dominance algorithm taken from paper "A Simple, Fast Dominance Algorithm" by Cooper et al.
    #
    def computeDominance(self):
        if len(self.basicBlocks) == 0 or self.haveDominance:
            return

        # Get the reverse post order
        postOrder = self.getPostOrder()
        reversePostOrder = postOrder[:-1]
        reversePostOrder.reverse()

        # Initialize the dominances
        entryNode = self.basicBlocks[0]
        doms = [-1]*len(postOrder)
        doms[entryNode.postOrderIndex] = entryNode.postOrderIndex
        changed = True
        while changed:
            changed = False
            for node in reversePostOrder:
                nodeIndex = node.postOrderIndex

                # Find first processed predecessor.
                newIdom = None
                for pred in node.predecessors:
                    if doms[pred.postOrderIndex] >= 0:
                        newIdom = pred.postOrderIndex
                        break
                assert newIdom is not None

                # Do for the others predecessors
                firstNewIdom = newIdom
                for pred in node.predecessors:
                    if pred.postOrderIndex != firstNewIdom and doms[pred.postOrderIndex] >= 0:
                        newIdom = self.dominanceIntersect(doms, pred.postOrderIndex, newIdom)

                if doms[nodeIndex] != newIdom:
                    changed = True
                    doms[nodeIndex] = newIdom


        # Store the immediate dominators
        for i in range(len(doms)):
            postOrder[i].immediateDominator = postOrder[doms[i]]
            if postOrder[i].immediateDominator is postOrder[i]:
                postOrder[i].immediateDominator = None

        # Reset the dominance frontier
        for n in self.basicBlocks:
            n.dominanceFrontier = set()

        # Build the dominance frontier
        for n in self.basicBlocks:
            preds = n.predecessors
            if len(preds) >= 2:
                for pred in preds:
                    runner = pred
                    end = n.immediateDominator
                    while runner is not end:
                        runner.dominanceFrontier.add(n)
                        runner = runner.immediateDominator

        self.haveDominance = True
                
    def computePostDominance(self):
        if len(self.basicBlocks) == 0 or self.havePostDominance:
            return

        # Get the reverse post order
        postOrder = self.getExitPostOrder()
        reversePostOrder = list(postOrder)
        reversePostOrder.reverse()

        fakeExit = BasicBlock(None)
        fakeExit.exitPostOrderIndex = len(postOrder)

        # Initialize the dominances
        doms = [-1]*(len(postOrder) + 1)
        doms[fakeExit.exitPostOrderIndex] = fakeExit.exitPostOrderIndex
        changed = True
        while changed:
            changed = False
            for node in reversePostOrder:
                nodeIndex = node.exitPostOrderIndex

                # Find first processed successor.
                newIdom = None
                for succ in node.getSuccessorsWithFakeExit(fakeExit):
                    if doms[succ.exitPostOrderIndex] >= 0:
                        newIdom = succ.exitPostOrderIndex
                        break
                assert newIdom is not None

                # Do for the others successor
                firstNewIdom = newIdom
                for succ in node.getSuccessorsWithFakeExit(fakeExit):
                    if succ.exitPostOrderIndex != firstNewIdom and doms[succ.exitPostOrderIndex] >= 0:
                        newIdom = self.dominanceIntersect(doms, succ.exitPostOrderIndex, newIdom)

                if doms[nodeIndex] != newIdom:
                    changed = True
                    doms[nodeIndex] = newIdom


        # Store the immediate post dominators
        for i in range(len(postOrder)):
            index = doms[i]
            if index == len(postOrder):
                postOrder[i].immediatePostDominator = None
            else:
                postOrder[i].immediatePostDominator = postOrder[index]

        # Reset the post dominance frontier
        for n in self.basicBlocks:
            n.postDominanceFrontier = set()

        # Build the dominance frontier
        for n in self.basicBlocks:
            preds = n.predecessors
            if len(preds) >= 2:
                for pred in preds:
                    runner = pred
                    end = n.immediatePostDominator
                    while runner is not end:
                        runner.postDominanceFrontier.add(n)
                        runner = runner.immediatePostDominator

        self.havePostDominance = True
        
    def dominanceIntersect(self, doms, n1, n2):
        finger1 = n1
        finger2 = n2
        while finger1 != finger2:
            while finger1 < finger2:
                finger1 = doms[finger1]
            while finger2 < finger1:
                finger2 = doms[finger2]
        return finger1

    def findLoops(self):
        if self.loops is not None:
            return self.loops
            
        # Reset the node loops
        for node in self.basicBlocks:
            node.loop = None
            
        # Compute the loops
        self.loops = []
        for node in self.getPreOrder():
            loop = ControlFlowLoop()
            if loop.findStartingAtBlock(node):
                self.loops.append(loop)
        return self.loops
        
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

# Basic block
# A basic block is a node in the program control flow graph.
class BasicBlock:
    def __init__(self, function, name=None):
        self.name = name
        self.function = function
        self.instructions = []
        self.successors = []
        self.predecessors = []
        self.index = -1
        self.postOrderIndex = -1
        self.exitPostOrderIndex = -1
        self.preOrderIndex = -1
        self.immediateDominator = None
        self.dominanceFrontier = set()
        self.immediatePostDominator = None
        self.postDominanceFrontier = set()
        self.mergeBlock = None
        self.loop = None
        if function is not None:
            function.addBasicBlock(self)
            if self.name is None:
                self.name = function.generateSymbol()

    
    def addInstruction(self, instruction):
        assert instruction.isInstruction()
        self.instructions.append(instruction)

        # If the instruction is a terminator, build the sucessors and predecessors
        if instruction.isTerminator():
            self.successors = instruction.getSuccessors()
            for successor in self.successors:
                successor.predecessors.append(self)

    def getLastInstruction(self):
        if len(self.instructions) == 0:
            return None
        return self.instructions[-1]

    def __str__(self):
        idomStr = ""
        if self.immediateDominator is not None:
            idomStr = "[idom: %s] " % self.immediateDominator.name

        domFrontierStr = ""
        if len(self.dominanceFrontier) > 0:
            domFrontierStr = "[domFr: "
            for node in self.dominanceFrontier:
                domFrontierStr += node.name + " "
            domFrontierStr += "]"

        ipdomStr = ""
        if self.immediatePostDominator is not None:
            ipdomStr = "[ipdom: %s] " % self.immediatePostDominator.name

        mergeStr = ""
        if self.mergeBlock is not None:
            mergeStr = "[merge: %s] " % self.mergeBlock.name
        
        pdomFrontierStr = ""
        if len(self.postDominanceFrontier) > 0:
            pdomFrontierStr = "[pdomFr: "
            for node in self.postDominanceFrontier:
                pdomFrontierStr += node.name + " "
            pdomFrontierStr += "]"

        result = '%s: %s%s%s%s%s\n'  % (self.name, idomStr, domFrontierStr, ipdomStr, pdomFrontierStr, mergeStr)
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

    def getSuccessors(self):
        return self.successors

    def getSuccessorsWithFakeExit(self, exit):
        if len(self.successors) == 0 and self.isExitBlock():
            return [exit]
        return self.successors

    def getPredecessors(self):
        return self.predecessors
        
    def dominatedBy(self, node):
        idom = self.immediateDominator
        while idom is not None:
            if idom is node:
                return True
            idom = idom.immediateDominator
        return False
        
    def dominates(self, node):
        return node.dominatedBy(self)

    def isExitBlock(self):
        last = self.getLastInstruction()
        return last is not None and last.isExit()
        
# Block builder
# A block builder is used to add instruction into a basic block.
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

    def getInsertBlock(self):
        return self.currentBlock
        
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

    def getElementReference(self, reference, indirectionIndex, name=None):
        return self.addInstruction(GetElementReferenceInstruction(reference, [indirectionIndex]), name)

    def getElementReferenceInd(self, reference, indirections, name=None):
        return self.addInstruction(GetElementReferenceInstruction(reference, indirections), name)

    def call(self, function, arguments, name=None):
        return self.addInstruction(CallInstruction(function, arguments), name)


class Instruction(Value):
    def __init__(self, parameters):
        self.ownerBlock = None
        self.name = ''
        self.parameters = parameters
        self.instructionReferences = []

        for param in parameters:
            param.addInstructionReference(self)

    def isInstruction(self):
        return True

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
        
    def accept(self, visitor):
        return visitor.visitAllocaInstruction(self)

    def getValueType(self):
        return self.valueType

    def getType(self):
        return self.type

    def definitionString(self):
        return 'alloca %s' % str(self.valueType)

class CallInstruction(Instruction):
    def __init__(self, function, arguments):
        Instruction.__init__(self, [function] + arguments)

        assert function.getType().isFunction()
        self.type = function.getType().returnType

    def accept(self, visitor):
        return visitor.visitCallInstruction(self)

    def getType(self):
        return self.type

    def getFunction(self):
        return self.parameters[0]

    def getArguments(self):
        return self.parameters[1:]

    def definitionString(self):
        argStr = ''
        for arg in self.getArguments():
            if len(argStr) > 0:
                argStr += ', '
            argStr += str(arg)
        return 'cal %s (%s)' % (str(self.getFunction()), argStr)

class LoadInstruction(Instruction):
    def __init__(self, reference):
        Instruction.__init__(self, [reference])

        assert reference.getType().isReference()
        self.type = reference.getType().baseType

    def accept(self, visitor):
        return visitor.visitLoadInstruction(self)

    def getType(self):
        return self.type

    def getReference(self):
        return self.parameters[0]

    def definitionString(self):
        return 'load from %s' % str(self.getReference())

class StoreInstruction(Instruction):
    def __init__(self, value, reference):
        Instruction.__init__(self, [value, reference])

    def accept(self, visitor):
        return visitor.visitStoreInstruction(self)

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

    def getSuccessors(self):
        pass

class JumpInstruction(TerminatorInstruction):
    def __init__(self, targetBlock):
        TerminatorInstruction.__init__(self, [])
        self.targetBlock = targetBlock

    def accept(self, visitor):
        return visitor.visitJumpInstruction(self)

    def getTargetBlock(self):
        return self.targetBlock

    def definitionString(self):
        return 'jump @%s' % self.targetBlock.name

    def getSuccessors(self):
        return [self.targetBlock]

class BranchInstruction(TerminatorInstruction):
    def __init__(self, condition, thenBlock, elseBlock):
        TerminatorInstruction.__init__(self, [condition])
        self.thenBlock = thenBlock
        self.elseBlock = elseBlock

        assert condition.getType().isBoolean()

    def accept(self, visitor):
        return visitor.visitBranchInstruction(self)

    def getCondition(self):
        return self.parameters[0]

    def getThenBlock(self):
        return self.thenBlock

    def getElseBlock(self):
        return self.elseBlock

    def definitionString(self):
        return 'branch %s then @%s else @%s' % (str(self.getCondition()), self.thenBlock.name, self.elseBlock.name)

    def getSuccessors(self):
        return [self.thenBlock, self.elseBlock]

class UnaryOperationInstruction(Instruction):
    def __init__(self, operation, operand):
        Instruction.__init__(self, [operand])
        self.operation = operation

    def accept(self, visitor):
        return visitor.visitUnaryOperationInstruction(self)

    def getOperand(self):
        return parameters[0]

    def definitionString(self):
        return '%s %s %s' % (self.operation, str(self.getOperand()))

class BinaryOperationInstruction(Instruction):

    def __init__(self, operation, left, right):
        Instruction.__init__(self, [left, right])
        self.operation = operation
        self.type = self.computeType(self.operation, left.getType(), right.getType())

    def accept(self, visitor):
        return visitor.visitBinaryOperationInstruction(self)

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
        
class GetElementReferenceInstruction(Instruction):

    def __init__(self, baseReference, indirections):
        Instruction.__init__(self, [baseReference])
        self.indirections = indirections
        self.computeType(baseReference.getType(), indirections)

    def accept(self, visitor):
        return visitor.visitGetElementReferenceInstruction(self)

    def getType(self):
        return self.type

    def getOffset(self):
        return self.offset

    def getBaseReference(self):
        return self.parameters[0]

    def definitionString(self):
        return 'getElementReference %s %s' % (self.getBaseReference(), self.indirections)
        
    def computeType(self, baseReference, indirections):
        assert baseReference.isReference()
        if len(indirections) == 0: return None

        currentType = baseReference.baseType
        offset = 0
        for index in indirections:
            offset += currentType.offsetAtIndex(index)
            currentType = currentType.typeAtIndex(index)

        self.type = ReferenceType.get(currentType, baseReference.isReadOnly())
        self.offset = offset

class UnreachableInstruction(TerminatorInstruction):
    def __init__(self):
        TerminatorInstruction.__init__(self, [])

    def accept(self, visitor):
        return visitor.visitUnreachableInstruction(self)

    def definitionString(self):
        return 'unreachable'

    def getSuccessors(self):
        return []
        
    def isExit(self):
        return True


class ReturnVoidInstruction(TerminatorInstruction):
    def __init__(self):
        TerminatorInstruction.__init__(self, [])

    def accept(self, visitor):
        return visitor.visitReturnVoidInstruction(self)

    def definitionString(self):
        return 'return void'

    def getSuccessors(self):
        return []
        
    def isExit(self):
        return True


class ReturnInstruction(TerminatorInstruction):
    def __init__(self, value):
        TerminatorInstruction.__init__(self, [value])

    def accept(self, visitor):
        return visitor.visitReturnInstruction(self)

    def getValue(self):
        return self.parameters[0]

    def definitionString(self):
        return 'return %s' % str(self.getValue())

    def getSuccessors(self):
        return []
        
    def isExit(self):
        return True


