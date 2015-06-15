FUNCTION_KIND_NORMAL = 'normal'
FUNCTION_KIND_VERTEX = 'vertex'
FUNCTION_KIND_FRAGMENT = 'fragment'
FUNCTION_KIND_GEOMETRY = 'geometry'
FUNCTION_KIND_TESSELLATION_CONTROL = 'tescontrol'
FUNCTION_KIND_TESSELLATION_EVALUATION = 'tesevaluation'
FUNCTION_KIND_COMPUTE = 'compute'

ARGUMENT_KIND_IN = 'in'
ARGUMENT_KIND_OUT = 'out'
ARGUMENT_KIND_INOUT = 'in'
ARGUMENT_KIND_NORMAL = 'normal'

PrimitiveTypes = {
}

OpaqueTypes = {
}

# Utility methods
def alignTo(offset, alignment):
    return (offset + alignment - 1) % alignment

def makePowerOfTwo(guess):
    highestBit = 0
    while(guess >> highestBit != 0):
        highestBit += 1

    if (1 << highestBit) == guess:
        return guess
    return 1 << (highestBit + 1)

# Type
class Type:
    def __init__(self):
        pass

    def isType(self):
        return True
        
    def isReference(self):
        return False

    def isVoid(self):
        return False
    
    def isInteger(self):
        return False

    def isBoolean(self):
        return False

    def isFloatingPoint(self):
        return False

    def isDouble(self):
        return False

    def isStructure(self):
        return False

    def isVector(self):
        return False

# Primitive type
class PrimitiveType(Type):
    def __init__(self, size, alignment):
        self.size = size
        self.alignment = alignment

class BooleanType(PrimitiveType):
    def __init__(self):
        PrimitiveType.__init__(self, 4, 4)
        assert 'bool' not in PrimitiveTypes
        PrimitiveTypes['bool'] = self

    def isBoolean(self):
        return True

    def __str__(self):
        return 'bool'

class IntegerType(PrimitiveType):
    def __init__(self, name, size, alignment, signed):
        PrimitiveType.__init__(self, size, alignment)
        assert name not in PrimitiveTypes
        PrimitiveTypes[name] = self

        self.name = name
        self.signed = signed

    def isInteger(self):
        return True
        
    def isSigned(self):
        return self.signed
        
    def isUnsigned(self):
        return not self.signed

    def __str__(self):
        return self.name

class FloatingPointType(PrimitiveType):
    def __init__(self, name, size, alignment):
        PrimitiveType.__init__(self, size, alignment)
        assert name not in PrimitiveTypes
        PrimitiveTypes[name] = self

        self.name = name

    def isFloatingPoint(self):
        return True

    def isDouble(self):
        return self.size == 8

    def __str__(self):
        return self.name

class OpaqueType(PrimitiveType):
    def __init__(self, name):
        PrimitiveType.__init__(self, 0, 0)
        self.name = name

        assert name not in OpaqueTypes
        OpaqueTypes[name] = self

    def __str__(self):
        return self.name

class VoidType(OpaqueType):
    def __init__(self):
        OpaqueType.__init__(self, 'void')

    def isVoid(self):
        return True

class SamplerType(OpaqueType):
    def __init__(self, name, dimensions, array):
        OpaqueType.__init__(self, name)
        self.dimensions = dimensions
        self.array = array

# Reference type
class ReferenceType(Type):
    referencesTypes = {}
    
    def __init__(self, baseType, readOnly=False):
        assert (baseType, readOnly) not in self.referencesTypes
        self.referencesTypes[baseType] = self
        self.baseType = baseType
        self.readOnly = readOnly

    def isReference(self):
        return True

    def isReadOnly(self):
        return self.readOnly

    def __str__(self):
        if self.readOnly:
            return "readonly " + str(self.baseType) + '&'
        return str(self.baseType) + '&'

    @classmethod
    def get(cls, baseType, readOnly=False):
        res = cls.referencesTypes.get((baseType, readOnly), None)
        if res is None: return cls(baseType, readOnly)
        return res

# Vector type
class VectorType(Type):
    vectorTypes = {}
    
    def __init__(self, baseType, elements):
        assert (baseType, elements) not in self.vectorTypes
        self.vectorTypes[(baseType, elements)] = self
        self.baseType = baseType
        self.elements = elements

        self.size = baseType.size*elements
        self.alignment = makePowerOfTwo(baseType.alignment*elements)

    def isVector(self):
        return True

    def __str__(self):
        return str(self.baseType) + str(self.elements)

    @classmethod
    def get(cls, baseType, elements):
        res = cls.vectorTypes.get((baseType, elements), None)
        if res is None: return cls(baseType, elements)
        return res
    
# Matrix type
class MatrixType(Type):
    matrixTypes = {}
    
    def __init__(self, baseType, rows, columns):
        assert (baseType, rows, columns) not in self.matrixTypes
        self.matrixTypes[(baseType, rows, columns)] = self
        self.baseType = baseType
        self.rows = rows
        self.columns = columns

    @classmethod
    def get(cls, baseType, rows, columns):
        res = cls.matrixTypes.get((baseType, rows, columns), None)
        if res is None: return cls(baseType, rows, columns)
        return res

# Attribute data
class Attribute:
    def __init__(self, name, arguments):
        self.name = name
        self.arguments = arguments

# Structure types
class StructureField:
    def __init__(self, name, fieldType, semantics):
        self.name = name
        self.fieldType = fieldType
        self.index = -1
        self.offset = -1
        self.location = -1
        self.semantics = semantics
        if 'location' in semantics:
            self.location = semantics['location']

class StructureType(Type):
    def __init__(self, name, fields, semantics):
        self.name = name
        self.fields = fields
        self.fieldDictionary = {}
        self.semantics = semantics
        self.size = 0
        self.alignment = 1

        index = 0        
        for f in self.fields:
            self.fieldDictionary[f.name] = f
            f.index = index
            index += 1

            # Add to my alignment
            self.alignment = max(self.alignment, f.fieldType.alignment)

            # Compute the offset and my new size
            self.size = alignTo(self.size, f.fieldType.alignment)
            f.offset = self.size;
            self.size += f.fieldType.size

        self.size = alignTo(self.size, self.alignment)

    def getField(self, name):
        return self.fieldDictionary[name]
        
    def isStructure(self):
        return True

    def offsetAtIndex(self, index):
        return self.fields[index].offset

    def typeAtIndex(self, index):
        return self.fields[index].fieldType

    def __str__(self):
        return 'struct ' + self.name

# Function type
class FunctionArgumentType:
    def __init__(self, argumentType, kind = ARGUMENT_KIND_NORMAL):
        self.type = argumentType
        self.kind = kind

    def __hash__(self):
        return hash(self.type) ^ hash(self.kind)

    def __eq__(self, o):
        return self.type == o.type and self.kind == o.kind

    def __str__(self):
        return '[%s]%s' % (self.kind, str(self.type))

class FunctionType(Type):
    functionTypes = {}

    def __init__(self, returnType, arguments, kind = FUNCTION_KIND_NORMAL):
        assert (kind, returnType, arguments) not in self.functionTypes
        self.functionTypes[(kind, returnType, arguments)] = self

        self.kind = kind
        self.returnType = returnType
        self.arguments = arguments

    def __str__(self):
        arguments = ''
        for arg in self.arguments:
            arguments += ' ' + str(arg)
        return '([%s]%s -> %s)' % (self.kind, arguments, str(self.returnType))

    def overloadName(self):
        result = '('
        for arg in self.arguments:
            result += ' ' + str(arg.type)
        result += ' )'
        return result

    def overloadTuple(self):
        return tuple(map(lambda arg: arg.type, self.arguments))

    @classmethod
    def get(cls, returnType, arguments, kind):
        arguments = tuple(arguments)
        result = cls.functionTypes.get((kind, returnType, arguments), None)
        if result is not None:
            return result
        return cls(returnType, arguments, kind)


BooleanType()
IntegerType('sbyte', 1, 1, True)
IntegerType('byte', 1, 1, False)
IntegerType('short', 2, 2, True)
IntegerType('ushort', 2, 2, False)
IntegerType('int', 4, 4, True)
IntegerType('uint', 4, 4, False)
FloatingPointType('float', 4, 4)
FloatingPointType('double', 8, 8)

VoidType()
SamplerType('sampler1D', 1, False)
SamplerType('sampler1DArray', 1, True)
SamplerType('sampler2D', 2, False)
SamplerType('sampler2DArray', 2, True)
SamplerType('sampler2DRect', 2, False)
SamplerType('sampler2DRectArray', 2, True)
SamplerType('samplerCube', 2, False)
SamplerType('samplerCubeArray', 2, True)
SamplerType('sampler3D', 3, False)
SamplerType('sampler3DArray', 3, True)

# Common basic types
BasicType_Bool = PrimitiveTypes['bool']
BasicType_Int = PrimitiveTypes['int']
BasicType_Float = PrimitiveTypes['float']
BasicType_Double = PrimitiveTypes['double']

BasicTypes = dict(PrimitiveTypes.items() + OpaqueTypes.items())
