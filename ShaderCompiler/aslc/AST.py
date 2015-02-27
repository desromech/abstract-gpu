# Node position
class NodePosition:
    def __init__(self, filename, lineno, column=None):
        self.filename = filename
        self.lineno = lineno
        self.column = column
        
    def __str__(self):
        if self.column is None:
            return '%s:%d' % (self.filename, self.lineno)
        return '%s:%d:%d' % (self.filename, self.lineno, self.column)

class AstNode:
    def __init__(self, position):
        self.position = position

class TranslationUnit(AstNode):
    def __init__(self, position, declarations):
        AstNode.__init__(self, position)
        self.declarations = declarations
        
    def accept(self, visitor):
        return visitor.visitTranslationUnit(self)

    def __str__(self):
        result = ''
        for decl in self.declarations:
            result += str(decl) + '\n'
        return result

class TypeNode(AstNode):
    def __init__(self, position, typeObject):
        AstNode.__init__(self, position)
        self.typeObject = typeObject

    def accept(self, visitor):
        return visitor.visitTypeNode(self)

    def __str__(self):
        return str(self.typeObject)

class FunctionKindNode(AstNode):
    def __init__(self, position, kind):
        AstNode.__init__(self, position)
        self.kind = kind

    def accept(self, visitor):
        return visitor.visitFunctionKindNode(self)

    def __str__(self):
        return 'kind(%s)' % str(self.kind)
                    
class FunctionArgumentNode(AstNode):
    def __init__(self, argumentType, name):
        AstNode.__init__(self, argumentType.position)
        self.argumentType = argumentType
        self.name = name

    def accept(self, visitor):
        return visitor.visitFunctionArgumentNode(self)
        
    def __str__(self):
        return str(self.argumentType) + ' ' + str(self.name)
                
class FunctionPrototype(AstNode):
    def __init__(self, position, functionKind, returnType, name, arguments):
        AstNode.__init__(self, position)
        self.functionKind = functionKind
        self.returnType = returnType
        self.name = name
        self.arguments = arguments

    def accept(self, visitor):
        return visitor.visitFunctionPrototype(self)

    def __str__(self):
        args = ''
        for arg in self.arguments:
            args += str(arg) + ','
        return '%s %s %s ( %s )' % (str(self.functionKind), str(self.returnType), str(self.name), args)

class FunctionDeclaration(AstNode):
    def __init__(self, prototype):
        AstNode.__init__(self, prototype.position)
        self.prototype = prototype

    def accept(self, visitor):
        return visitor.visitFunctionDeclaration(self)

    def __str__(self):
        return str(self.prototype)

class FunctionDefinition(AstNode):
    def __init__(self, prototype, body):
        AstNode.__init__(self, prototype.position)
        self.prototype = prototype
        self.body = body

    def accept(self, visitor):
        return visitor.visitFunctionDefinition(self)

    def __str__(self):
        return str(self.prototype) + ' -> ' + str(self.body)
        
class Statement(AstNode):
    def __init__(self, position):
        AstNode.__init__(self, position)

class VariablesDeclaration(Statement):
    def __init__(self, typeExpression, variables):
        AstNode.__init__(self, typeExpression.position)
        self.typeExpression = typeExpression
        self.variables = variables

    def accept(self, visitor):
        return visitor.visitVariablesDeclaration(self)
        
class VariableDeclaration(Statement):
    def __init__(self, position, identifier, initialValue):
        AstNode.__init__(self, position)
        self.identifier = identifier
        self.initialValue = initialValue

    def accept(self, visitor):
        return visitor.visitVariableDeclaration(self)
        
class NullStatement(Statement):
    def __str__(self):
        return ';'

    def accept(self, visitor):
        return visitor.visitNullStatement(self)

class ExpressionStatement(Statement):
    def __init__(self, expression):
        Statement.__init__(self, expression.position)
        self.expression = expression
        
    def accept(self, visitor):
        return visitor.visitExpressionStatement(self)
    
class BlockStatement(Statement):
    def __init__(self, position, body):
        Statement.__init__(self, position)
        self.body = body
        
    def __str__(self):
        result = '{\n'
        for stmn in self.body:
            result += str(stmn) + '\n'
        return result + '}\n'

    def accept(self, visitor):
        return visitor.visitBlockStatement(self)

class IfStatement(Statement):
    def __init__(self, position, condition, thenStatement, elseStatement):
        Statement.__init__(self, position)
        self.condition = condition
        self.thenStatement = thenStatement
        self.elseStatement = elseStatement

    def accept(self, visitor):
        return visitor.visitIfStatement(self)

    def __str__(self):
        return 'if (%s)\n%s\nelse\n%s' % (str(self.condition), str(self.thenStatement), str(self.elseStatement))
 
class WhileStatement(Statement):
    def __init__(self, position, condition, body):
        Statement.__init__(self, position)
        self.condition = condition
        self.body = body

    def accept(self, visitor):
        return visitor.visitWhileStatement(self)

    def __str__(self):
        return 'while (%s)\n%s' % (str(self.condition), str(self.body))

class DoWhileStatement(Statement):
    def __init__(self, position, condition, body):
        Statement.__init__(self, position)
        self.condition = condition
        self.body = body

    def accept(self, visitor):
        return visitor.visitDoWhileStatement(self)

    def __str__(self):
        return 'do\n%s\nwhile (%s)' % (str(self.body), str(self.condition))

class ForStatement(Statement):
    def __init__(self, position, initialization, condition, increment, body):
        Statement.__init__(self, position)
        self.initialize = initialization
        self.condition = condition
        self.increment = increment
        self.body = body

    def accept(self, visitor):
        return visitor.visitForStatement(self)

class BreakStatement(Statement):
    def __str__(self):
        return 'break'
    
    def accept(self, visitor):
        return visitor.visitBreakStatement(self)

class ContinueStatement(Statement):
    def __str__(self):
        return 'continue'

    def accept(self, visitor):
        return visitor.visitContinueStatement(self)
    
class DiscardStatement(Statement):
    def __str__(self):
        return 'discard'

    def accept(self, visitor):
        return visitor.visitDiscardStatement(self)

class ReturnStatement(Statement):
    def __init__(self, position, expression=None):
        Statement.__init__(self, position)
        self.expression = expression
        
    def __str__(self):
        if self.expression is not None:
            return 'return ' + str(self.expression)
        return 'return'

    def accept(self, visitor):
        return visitor.visitReturnStatement(self)

class Expression(AstNode):
    pass
    
class BinaryExpression(Expression):
    def __init__(self, position, operation, left, right):
        Expression.__init__(self, position)
        self.operation = operation
        self.left = left
        self.right = right

    def accept(self, visitor):
        return visitor.visitBinaryExpression(self)

class BinaryAssignmentExpression(Expression):
    def __init__(self, position, operation, left, right):
        Expression.__init__(self, position)
        self.operation = operation
        self.left = left
        self.right = right

    def accept(self, visitor):
        return visitor.visitBinaryAssignmentExpression(self)

class UnaryExpression(Expression):
    def __init__(self, position, operation, operand):
        Expression.__init__(self, position)
        self.operation = operation
        self.operand = operand
        
    def accept(self, visitor):
        return visitor.visitUnaryExpression(self)

class IdentifierExpr(Expression):
    def __init__(self, position, identifier):
        Expression.__init__(self, position)
        self.identifier = identifier

    def accept(self, visitor):
        return visitor.visitIdentifierExpression(self)

class BooleanConstant(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)
        
    def accept(self, visitor):
        return visitor.visitBooleanConstant(self)

class IntegerConstant(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)
        
    def accept(self, visitor):
        return visitor.visitIntegerConstant(self)

class RealConstant(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)

    def accept(self, visitor):
        return visitor.visitRealConstant(self)

class StringLiteral(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)

    def accept(self, visitor):
        return visitor.visitStringLiteral(self)

