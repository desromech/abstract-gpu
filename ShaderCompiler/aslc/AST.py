# Functions kinds
FUNCTION_KIND_NORMAL = 'normal'
FUNCTION_KIND_VERTEX = 'vertex'
FUNCTION_KIND_FRAGMENT = 'fragment'
FUNCTION_KIND_GEOMETRY = 'geometry'
FUNCTION_KIND_TESSELLATION_CONTROL = 'tescontrol'
FUNCTION_KIND_TESSELLATION_EVALUATION = 'tesevaluation'
FUNCTION_KIND_COMPUTE = 'compute'

# Node position
class NodePosition:
    def __init__(self, filename, lineno, column=None):
        self.filename = filename
        self.lineno = lineno
        self.column = column
        
    def __str__(self):
        if column is None:
            return self.filename + ':' + self.lineno
        return '%s:%d:%d' % (self.filename, self.lineno, self.column)

class AstNode:
    def __init__(self, position):
        self.position = position

class Module(AstNode):
    def __init__(self, position, declarations):
        AstNode.__init__(self, position)
        self.declarations = declarations
        
    def __str__(self):
        result = ''
        for decl in self.declarations:
            result += str(decl) + '\n'
        return result

class TypeNode(AstNode):
    def __init__(self, position, typeObject):
        AstNode.__init__(self, position)
        self.typeObject = typeObject

    def __str__(self):
        return str(self.typeObject)

class FunctionKindNode(AstNode):
    def __init__(self, position, kind):
        AstNode.__init__(self, position)
        self.kind = kind
        
    def __str__(self):
        return 'kind(%s)' % str(self.kind)
                    
class FunctionArgument(AstNode):
    def __init__(self, argumentType, name):
        AstNode.__init__(self, argumentType.position)
        self.argumentType = argumentType
        self.name = name
        
    def __str__(self):
        return str(self.argumentType) + ' ' + str(self.name)
                
class FunctionPrototype(AstNode):
    def __init__(self, position, functionKind, returnType, name, arguments):
        AstNode.__init__(self, position)
        self.functionKind = functionKind
        self.returnType = returnType
        self.name = name
        self.arguments = arguments
        
    def __str__(self):
        args = ''
        for arg in self.arguments:
            args += str(arg) + ','
        return '%s %s %s ( %s )' % (str(self.functionKind), str(self.returnType), str(self.name), args)

class FunctionDeclaration(AstNode):
    def __init__(self, prototype):
        AstNode.__init__(self, prototype.position)
        self.prototype = prototype

    def __str__(self):
        return str(self.prototype)

class FunctionDefinition(AstNode):
    def __init__(self, prototype, body):
        AstNode.__init__(self, prototype.position)
        self.prototype = prototype
        self.body = body

    def __str__(self):
        return str(self.prototype) + ' -> ' + str(self.body)
        
class Statement(AstNode):
    def __init__(self, position):
        AstNode.__init__(self, position)
        
class NullStatement(Statement):
    def __str__(self):
        return ';'
    
class BlockStatement(Statement):
    def __init__(self, position, body):
        Statement.__init__(self, position)
        self.body = body
        
    def __str__(self):
        result = '{\n'
        for stmn in self.body:
            result += str(stmn) + '\n'
        return result + '}\n'

class IfStatement(Statement):
    def __init__(self, position, condition, thenStatement, elseStatement):
        Statement.__init__(self, position)
        self.condition = condition
        self.thenStatement = thenStatement
        self.elseStatement = elseStatement

    def __str__(self):
        return 'if (%s)\n%s\nelse\n%s' % (str(self.condition), str(self.thenStatement), str(self.elseStatement))
 
class WhileStatement(Statement):
    def __init__(self, position, condition, body):
        Statement.__init__(self, position)
        self.condition = condition
        self.body = body

    def __str__(self):
        return 'while (%s)\n%s' % (str(self.condition), str(self.body))

class DoWhileStatement(Statement):
    def __init__(self, position, condition, body):
        Statement.__init__(self, position)
        self.condition = condition
        self.body = body

    def __str__(self):
        return 'do\n%s\nwhile (%s)' % (str(self.body), str(self.condition))

class ForStatement(Statement):
    def __init__(self, position, initialization, condition, increment, body):
        Statement.__init__(self, position)
        self.initialize = initialization
        self.condition = condition
        self.increment = increment
        self.body = body

class BreakStatement(Statement):
    def __str__(self):
        return 'break'
    
class ContinueStatement(Statement):
    def __str__(self):
        return 'continue'
    
class DiscardStatement(Statement):
    def __str__(self):
        return 'discard'

class ReturnStatement(Statement):
    def __init__(self, position, expression=None):
        Statement.__init__(self, position)
        self.expression = expression
        
    def __str__(self):
        if self.expression is not None:
            return 'return ' + str(self.expression)
        return 'return'

class Expression(AstNode):
    pass
    
class BinaryExpression(Expression):
    def __init__(self, position, operation, left, right):
        Expression.__init__(self, position)
        self.operation = operation
        self.left = left
        self.right = right

class UnaryExpression(Expression):
    def __init__(self, position, operation, operand):
        Expression.__init__(self, position)
        self.operation = operation
        self.operand = operand
        
class IdentifierExpr(Expression):
    def __init__(self, position, identifier):
        Expression.__init__(self, position)
        self.identifier = identifier

class BooleanConstant(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)
        
class IntegerConstant(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)
        
class RealConstant(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)
        
class StringLiteral(Expression):
    def __init__(self, position, value):
        Expression.__init__(self, position)
        self.value = value
        
    def __str__(self):
        return str(self.value)
