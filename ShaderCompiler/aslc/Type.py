
# Type
class Type:
    def __init__(self):
        pass
    
# Primitive type
class PrimitiveType(Type):
    def __init__(self, size, alignment):
        self.size = size
        self.alignment = alignment

class IntegerType(PrimitiveType):
    def __init__(self, size, alignment, signed):
        PrimitiveType.__init__(self, size, alignment)
        self.signed = signed

class FloatingPointType(PrimitiveType):
    def __init__(self, size, alignment):
        PrimitiveType.__init__(self, size, alignment)

class OpaqueType(PrimitiveType):
    def __init__(self):
        PrimitiveType.__init__(self, 0, 0)

class VoidType(OpaqueType):
    def __init__(self):
        OpaqueType.__init__(self)
        
    def __str__(self):
        return 'void'

class SamplerType(OpaqueType):
    def __init__(self, dimensions, array):
        OpaqueType.__init__(self)
        self.dimensions = dimensions
        self.array = array

# Vector type
class VectorType(Type):
    vectorTypes = {}
    
    def __init__(self, baseType, elements):
        assert (baseType, elements) not in self.vectorTypes
        self.vectorTypes[(baseType, elements)] = self
        self.baseType = baseType
        self.elements = elements

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
    
PrimitiveTypes = {
    'sbyte' : IntegerType(1, 1, True),
    'byte' : IntegerType(1, 1, False),
    'short' : IntegerType(2, 2, True),
    'ushort' : IntegerType(2, 2, False),
    'int' : IntegerType(4, 4, True),
    'uint' : IntegerType(4, 4, False),
    'float' : FloatingPointType(4, 4),
    'double' : FloatingPointType(8, 8),
}

OpaqueTypes = {
    'void' : VoidType(),
    'sampler1D' : SamplerType(1, False),
    'sampler1DArray' : SamplerType(1, True),
    'sampler2D' : SamplerType(2, False),
    'sampler2DArray' : SamplerType(2, True),
    'sampler2DRect' : SamplerType(2, False),
    'sampler2DRectArray' : SamplerType(2, True),
    'samplerCube' : SamplerType(2, False),
    'samplerCubeArray' : SamplerType(2, True),
    'sampler3D' : SamplerType(3, False),
    'sampler3DArray' : SamplerType(3, True),
}

BasicTypes = dict(PrimitiveTypes.items() + OpaqueTypes.items())
