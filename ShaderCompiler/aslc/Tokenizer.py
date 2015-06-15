import ply.lex as lex
from AST import NodePosition
from Type import *

tokens = (
    # keywords
    "TRUE",
    "FALSE",

    # Control flow
    "IF",
    "ELSE",
    "WHILE",
    "FOR",
    "DO",
    "BREAK",
    "CONTINUE",
    "DISCARD",
    "RETURN",
    
    # Qualifiers
    "UNIFORM",
    "IN",
    "OUT",
    "INOUT",
    "KERNEL",
    
    # Operators
    "ASSIGN",
    "ASSIGN_PLUS",
    "ASSIGN_MINUS",
    "ASSIGN_MULTIPLY",
    "ASSIGN_DIVIDE",
    "ASSIGN_REMAINDER",
    
    "ASSIGN_BITAND",
    "ASSIGN_BITOR",
    "ASSIGN_BITXOR",
    "ASSIGN_SHIFTLEFT",
    "ASSIGN_SHIFTRIGHT",
    
    "PLUS",
    "MINUS",
    "MULTIPLY",
    "DIVIDE",
    "REMAINDER",
   
    "LT",
    "LE",
    "EQ",
    "NE",
    "GT",
    "GE",
    
    "LAND",
    "LOR",
    "LNOT",
    "BITAND",
    "BITOR",
    "BITNOT",
    "BITXOR",
    "SHIFTLEFT",
    "SHIFTRIGHT",
       
    # Syntactic components
    "LPAREN",
    "RPAREN",
    "LBRACKET",
    "RBRACKET",
    "LCBRACKET",
    "RCBRACKET",
    
    "DOT",
    "COMMA",
    "SEMICOLON",
    "COLON",
    
    # Components
    "TYPE",
    "IDENTIFIER",
    "REAL",
    "INTEGER",
    "STRING",

    # Types
    "STRUCT",
)

reserved = {
    'if' : 'IF',
    'else' : 'ELSE',
    'while' : 'WHILE',
    'do' : 'DO',
    'for' : 'FOR',
    'break' : 'BREAK',
    'continue' : 'CONTINUE',
    'return' : 'RETURN',

    'uniform' : 'UNIFORM',
    'in' : 'IN',
    'out' : 'OUT',
    'inout' : 'INOUT',
    'kernel' : 'KERNEL',
    
    'true': 'TRUE',
    'false': 'FALSE',

    'and' : 'LAND',
    'or' : 'LOR',
    'not' :'LNOT',
    
    'bitand' : 'BITAND',
    'bitnot' : 'BITNOT',
    'bitor' : 'BITOR',
    'bitxor' : 'BITXOR',

    'struct' : 'STRUCT',
}

STRING_ESCAPE_MAP = {
    'a' : '\a',
    'n' : '\n',
    'r' : '\r',
    't' : '\t',
    'f' : '\f',
}

class Token:
    def __init__(self, position, value):
        self.position = position
        self.value = value

def currentPosition(lexer):
    return NodePosition(lexer.filename, lexer.lineno)
    
def addPosition(token):
    token.value = Token(currentPosition(token.lexer), token.value)
    return token
    
def unescapeString(val):
    result = ''
    i = 1
    while i < len(val) - 1:
        c = val[i]
        if c == '\\':
            i += 1
            c = val[i]
            c = STRING_ESCAPE_MAP.get(c, c)
        result += c          
        i += 1
    return result
    
def t_ASSIGN_PLUS(t):
    r'\+='
    return addPosition(t)

def t_ASSIGN_MINUS(t):
    r'-='
    return addPosition(t)

def t_ASSIGN_MULTIPLY(t):
    r'\*='
    return addPosition(t)

def t_ASSIGN_DIVIDE(t):
    r'/='
    return addPosition(t)

def t_ASSIGN_REMAINDER(t):
    r'%='
    return addPosition(t)

def t_ASSIGN_BITAND(t):
    r'&='
    return addPosition(t)

def t_ASSIGN_BITOR(t):
    r'\|='
    return addPosition(t)

def t_ASSIGN_BITXOR(t):
    r'^='
    return addPosition(t)

def t_ASSIGN_SHIFTLEFT(t):
    r'<<='
    return addPosition(t)

def t_ASSIGN_SHIFTRIGHT(t):
    r'>>='
    return addPosition(t)

def t_PLUS(t):
    r'\+'
    return addPosition(t)
    
def t_MINUS(t):
    r'-'
    return addPosition(t)
    
def t_MULTIPLY(t):
    r'\+'
    return addPosition(t)
    
def t_DIVIDE(t):
    r'/'
    return addPosition(t)
    
def t_REMAINDER(t):
    r'%'
    return addPosition(t)

def t_DOT(t):
    r'\.'
    return addPosition(t)

def t_COMMA(t):
    r','
    return addPosition(t)
    
def t_SEMICOLON(t):
    r';'
    return addPosition(t)

def t_COLON(t):
    r':'
    return addPosition(t)

def t_LE(t):
    r'<='
    return addPosition(t)
    
def t_LT(t):
    r'<'
    return addPosition(t)
    
def t_EQ(t):
    r'=='
    return addPosition(t)
    
def t_ASSIGN(t):
    r'='
    return addPosition(t)

def t_NE(t):
    r'!='
    return addPosition(t)
    
def t_GE(t):
    r'>='
    return addPosition(t)
    
def t_GT(t):
    r'>'
    return addPosition(t)

def t_LAND(t):
    r'&&'
    return addPosition(t)
    
def t_LOR(t):
    r'\|\|'
    return addPosition(t)
    
def t_LNOT(t):
    r'!'
    return addPosition(t)

def t_BITAND(t):
    r'&'
    return addPosition(t)
    
def t_BITOR(t):
    r'\|'
    return addPosition(t)
    
def t_BITNOT(t):
    r'~'
    return addPosition(t)
    
def t_BITXOR(t):
    r'\^'
    return addPosition(t)
    
def t_SHIFTLEFT(t):
    r'<<'
    return addPosition(t)
    
def t_SHIFTRIGHT(t):
    r'>>'
    return addPosition(t)

def t_LPAREN(t):
    r'\('
    return addPosition(t)
    
def t_RPAREN(t):
    r'\)'
    return addPosition(t)

def t_LBRACKET(t):
    r'\['
    return addPosition(t)
    
def t_RBRACKET(t):
    r'\]'
    return addPosition(t)

def t_LCBRACKET(t):
    r'\{'
    return addPosition(t)
    
def t_RCBRACKET(t):
    r'\}'
    return addPosition(t)

def t_MATRIX_TYPE(t):
    r'(?P<type>[_a-zA-Z][_a-zA-Z]+)(?P<rows>[0-9]+)x(?P<columns>[0-9]+)'
    match = t.lexer.lexmatch 
    typ = match.group('type')
    baseType = PrimitiveTypes.get(typ, None)
    if baseType is None:
        t.type = 'IDENTIFIER'
        return addPosition(t)
    
    rows = int(match.group('rows'))
    columns = int(match.group('columns'))
    if not (rows >= 2 and rows <= 4 and columns >= 2 and columns <= 4):
        print 'Invalid matrix type "%s"' % t.value
        return

    t.value = MatrixType.get(baseType, rows, columns)
    t.type = 'TYPE'
    return addPosition(t)
    
def t_VECTOR_TYPE(t):
    r'(?P<type>[_a-zA-Z][_a-zA-Z]+)(?P<elements>[0-9]+)'
    match = t.lexer.lexmatch 
    typ = match.group('type')
    elements = int(match.group('elements'))
    baseType = PrimitiveTypes.get(typ, None)
    if baseType is None:
        t.type = 'IDENTIFIER'
        return addPosition(t)
    
    if not (elements >= 2 and elements <= 4):
        print 'Invalid vector type "%s"' % t.value
        return

    t.value = VectorType.get(baseType, elements)
    t.type = 'TYPE'
    return addPosition(t)    
    
def t_IDENTIFIER(t):
    r'[_a-zA-Z][_a-zA-Z0-9]*'
    primitiveType = BasicTypes.get(t.value, None)
    if primitiveType is not None:
        t.type = 'TYPE'
        t.value = primitiveType
    else:
        t.type = reserved.get(t.value, 'IDENTIFIER')
    return addPosition(t)

def t_REAL(t):
    r'[0-9]+\.[0-9]*(?:[eE][+-]?[0-9]+)?|.[0-9]+(?:[eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+'
    t.value = float(t.value)
    return addPosition(t)

def t_INTEGER(t):
    r'[0-9]+'
    t.value = int(t.value)
    return addPosition(t)
    
def t_STRING(t):
    r'"(?:[^"\\]|(?:\\.))*"'
   
    t.value = unescapeString(t.value)
    return addPosition(t)
    
def t_singleLineComment(t):
    r'//.*\n'
    t.lexer.lineno += len(t.value)

def t_multiLineComment(t):
    r'/\*.*\*/\n'
    t.lexer.lineno += t.value.count('\n')
    
def t_linemarker(t):
    r'\#\s+(?P<lineno>[0-9]+)\s+(?P<filename>"(?:[^"\\]|(?:\\.))*").*\n'
    match = t.lexer.lexmatch
    t.lexer.lineno = int(match.group('lineno'))
    t.lexer.filename = unescapeString(match.group('filename'))

def t_preprocessor(t):
    r'\#.*\n'
    t.lexer.lineno += len(t.value)

def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)
    
# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule 
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)
    
lex.lex()

def dumpTokens(string):
    lex.input(string)
    for tok in iter(lex.token, None):
        print tok
