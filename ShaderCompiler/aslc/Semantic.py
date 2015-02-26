import AST
from SSA import *
from Type import *
from Parser import parser

class SemanticError(Exception):
    def __init__(self, position, message):
        self.position = position
        self.message = message

    def __str__(self):
        return "%s: Error: %s" % (str(self.position), self.message)

def error(node, message):
    raise SemanticError(node.position, message)

class Scope:
    def __init__(self, parent):
        self.symbols = {}
        self.parent = parent

    def addSymbol(self, symbol, value, node=None):
        if symbol in self.symbols:
            error(position, 'redefining existing symbol "%s".' % symbol)
        self.symbols[symbol] = value

    def lookSymbol(self, name):
        return self.symbols.get(name, None)

    def lookSymbolRecursively(self, symbol):
        value = self.symbols.get(symbol, None)
        if value is None and self.parent is not None: 
            return self.parent.lookSymbolRecursively(symbol)
        return value

class FunctionGroup:
    def __init__(self):
        self.functions = {}

    def addFunction(self, name, function):
        self.functions[name] = function

# Semantic analysis common
class SemanticAnalysis:
    def __init__(self, scope):
        self.scope = scope

    def visitIdentifierExpression(self, expr):
        result =  self.scope.lookSymbolRecursively(expr.identifier)
        if result is None:
            error('failed to find identifier "%s"' % expr.identifier)
        return result

    def visitTypeNode(self, node):
        return node.typeObject

    def visitBooleanConstant(self, constant):
        return ConstantValue.get(BasicType_Bool, constant.value)

# Function specific semantic analysis.
class FunctionSemanticAnalysis(SemanticAnalysis):
    def __init__(self, function, parentScope):
        SemanticAnalysis.__init__(self, Scope(parentScope))
        self.function = function
        self.functionType = function.getType()
        self.breakBlock = None
        self.continueBlock = None

    def storeBreakAndContinue(self):
        return (self.breakBlock, self.continueBlock)

    def loadBreakAndContinue(self, breakAndContinue):
        self.breakBlock, self.continueBlock = breakAndContinue

    def evalReferences(self, value):
        tpe = value.getType()
        if tpe.isReference():
            return self.builder.load(value)
        else:
            return value

    def coerceInto(self, value, targetType, where):
        # If the target type is a reference, the value has to have the exact same type
        if targetType.isReference():
            if value.getType() != targetType:
                error(where, 'cannot coerce reference types. Use the exact same type in the variable when passing a reference')
            return value

        # Eval the references.
        value = self.evalReferences(value)

        # TODO: Support coercions
        if value.getType() != targetType:
            error(statement, 'cannot coerce %s into %s' % ( str(value.getType()), str(returnType)))

        return value

    def addArguments(self, prototype):
        # Iterate through the arguemtns
        functionType = self.function.type
        functionArguments = self.function.arguments
        prototypeArguments = prototype.arguments

        for i in range(len(functionArguments)):
            arg = functionArguments[i]
            protoArg = prototypeArguments[i]

            # Allocate the argument variable
            arg.name = protoArg.name
            location = self.declarationsBuilder.alloca(arg.type, 'arguments.' + arg.name)
            self.scope.addSymbol(arg.name, location, prototype)

            # Set the argument variable initial value
            self.builder.store(arg, location)

    def visitIfStatement(self, statement):
        # Create the if blocks
        thenBlock = BasicBlock(self.function, 'ifThen')
        elseBlock = None
        if statement.elseStatement is not None:
            elseBlock = BasicBlock(self.function, 'ifElse')
        continueBlock = BasicBlock(self.function, 'ifContinue')
        if elseBlock is None:
            elseBlock = continueBlock

        # Coerce the condition into a boolean
        condition = self.coerceInto(statement.condition.accept(self), BasicType_Bool, statement)
        self.builder.branch(condition, thenBlock, elseBlock)

        # Compile the then block
        self.builder.setInsertBlock(thenBlock)
        statement.thenStatement.accept(self)

        if not self.builder.isLastTerminator:
            self.builder.jump(continueBlock)

        # Compile the else block
        if statement.elseStatement is not None:
            build.setInsertBlock(elseBlock)
            statement.elseStatement.accept(self)

            if not self.builder.isLastTerminator:
                self.builder.jump(continueBlock)

        # Continue compiling after the if
        self.builder.setInsertBlock(continueBlock)

    def visitWhileStatement(self, statement):
        # Create the continue and break blocks
        continueBlock = BasicBlock(self.function, 'whileCond')
        bodyBlock = BasicBlock(self.function, 'whileBody')
        breakBlock = BasicBlock(self.function, 'whileEnd')

        #Store the old break and continue
        oldBreakContinue = self.storeBreakAndContinue()
        self.loadBreakAndContinue((breakBlock, continueBlock))

        # Enter into the loop.
        self.builder.jump(continueBlock)
        self.builder.setInsertBlock(continueBlock)

        # Evaluate the while condition
        condition = self.coerceInto(statement.condition.accept(self), BasicType_Bool, statement)
        self.builder.branch(condition, bodyBlock, breakBlock)

        # Evaluate the while body
        self.builder.setInsertBlock(bodyBlock)
        statement.body.accept(self)

        # Continue looping
        if not self.builder.isLastTerminator():
            self.builder.jump(continueBlock)

        # Restore the old break and continue
        self.loadBreakAndContinue(oldBreakContinue)

        # Continue with the rest of the program
        self.builder.setInsertBlock(continueBlock)

    def visitBreakStatement(self, statement):
        if self.breakBlock is None:
            error(statement, 'cannot use break here. No loop to end.')
        self.builder.jump(self.breakBlock)


    def visitContinueStatement(self, statement):
        if self.continueBlock is None:
            error(statement, 'cannot use break here. No loop to continue.')
        self.builder.jump(self.continueBlock)

    def visitBlockStatement(self, statement):

        # Stablish a new scope for the block
        oldScope = self.scope
        self.scope = Scope(oldScope)

        # Evaluate the block children
        for child in statement.body:
            child.accept(self)

        # Restore the old scope
        self.scope = oldScope

    def visitReturnStatement(self, statement):
        returnType = self.functionType.returnType
        expression = statement.expression

        # Handle void return in a special way
        if returnType.isVoid():
            if expression is not None:
                value = self.evalReferences(expression.accept(self))
                if not value.getType().isVoid():
                    error(statement, 'returning a value in a function that returns void')
            self.builder.returnVoid()
            return

        # Ensure there is a return value
        if expression is None:
            error(statement, 'a return value is expected')

        value = self.coerceInto(expression.accept(self), returnType, statement)
        self.builder.returnValue(value)


    def compile(self, definition):
        # Create the entry basic block and the instruction builder
        declarations = BasicBlock(self.function, 'declarations')
        entry = BasicBlock(self.function, 'entry')
        self.declarationsBuilder = BlockBuilder(declarations)
        self.builder = BlockBuilder(entry)

        # Declare the argument variables, and add them to the scope
        self.addArguments(definition.prototype)

        # Compile the body
        for node in definition.body:
            node.accept(self)

        # Jump from the declaration into the actual entry
        self.declarationsBuilder.jump(entry)

        # Add the implicit final return
        self.implicitReturn(definition)

    def implicitReturn(self, definition):
        if self.builder.isLastTerminator():
            return

        if self.functionType.returnType.isVoid():
            self.builder.returnVoid()
        else:
            error(definition, 'the function has to always return a value')

# Moduule specific semantic analysis.
class ModuleSemanticAnalysis(SemanticAnalysis):
    def __init__(self):
        SemanticAnalysis.__init__(self, Scope(None))
        self.module = Module()
        self.errorCount = 0

    def visitTranslationUnit(self, translationUnit):
        for declaration in translationUnit.declarations:
            declaration.accept(self)

    def visitFunctionPrototype(self, prototype):
        group = self.scope.lookSymbol(prototype.name)
        functionType = self.makeFunctionType(prototype)

        # Get the existing function
        function = None
        if group is None:
            group = FunctionGroup()
            self.scope.addSymbol(prototype.name, group)
        else:
            function = group.getFunction(functionType.overloadTuple())

        # Add the function
        if function is None:
            function = Function(prototype.name + functionType.overloadName(), functionType)
            self.module.addGlobalValue(function)
            group.addFunction(functionType.overloadTuple(), function)
        return function

    def visitFunctionDefinition(self, definition):
        # Declare the function by visiting the prototype
        function = definition.prototype.accept(self)

        # Ensure this is a declaration
        if not function.isDeclaration():
            error("redefining existing function '%s'", function.name)

        # Compile the function
        functionAnalysis = FunctionSemanticAnalysis(function, self.scope)
        functionAnalysis.compile(definition)
        return function

    def visitFunctionKindNode(self, node):
        return node.kind

    def visitFunctionArgumentNode(self, node):
        return FunctionArgumentType(node.argumentType.accept(self))

    def makeFunctionType(self, prototype):
        kind = prototype.functionKind.accept(self)
        returnType = prototype.returnType.accept(self)
        arguments = map (lambda arg: arg.accept(self), prototype.arguments)
        return FunctionType.get(returnType, arguments, kind)

    def performOn(self, ast):
        try:
            ast.accept(self)
        except SemanticError as semanticError:
            print semanticError
            self.errorCount += 1

def compileString(text):
    ast = parser.parse(text)
    analysis = ModuleSemanticAnalysis()
    analysis.performOn(ast)
    return analysis.module

