from lxml import etree

def getOptionalAttribute(node, attr, default):
    if attr not in node.keys():
        return default
    return node.get(attr)
    
class Typedef:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.ctype = xmlNode.get('ctype')
        
    def accept(self, visitor):
        return visitor.visitTypedef(self)

class Field:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.type = xmlNode.get('type')

    def accept(self, visitor):
        return visitor.visitField(self)
      
class Struct:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.fields = []
        self.loadFields(xmlNode)
        
    def accept(self, visitor):
        return visitor.visitStruct(self)

    def loadFields(self, node):
        for child in node:
            if child.tag == 'field':
                self.fields.append(Field(child))

class Enum:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.ctype = xmlNode.get('ctype')
        self.constants = []
        self.loadConstants(xmlNode)

    def accept(self, visitor):
        return visitor.visitEnum(self)

    def loadConstants(self, node):
        for child in node:
            if child.tag == 'constant':
                self.constants.append(Constant(child))

class Constant:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.type = getOptionalAttribute(xmlNode, 'type', 'int')
        self.value = xmlNode.get('value')
        
    def accept(self, visitor):
        return visitor.visitConstant(self)

class SelfArgument:
    def __init__(self, clazz):
        self.name = clazz.name
        self.type = clazz.name + '*'
        
class Argument:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.type = xmlNode.get('type')
        
class Function:
    def __init__(self, xmlNode, clazz = None):
        self.name = xmlNode.get('name')
        self.cname = getOptionalAttribute(xmlNode, 'cname', self.name)
        self.returnType = xmlNode.get('returnType')
        self.clazz = clazz
        self.arguments = []
        self.loadArguments(xmlNode)

    def accept(self, visitor):
        return visitor.visitFunction(self)

    def loadArguments(self, xmlNode):
        for child in xmlNode:
            if child.tag == 'arg':
                self.arguments.append(Argument(child))
                
class Interface:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        self.methods = []
        self.loadMethods(xmlNode)
        
    def accept(self, visitor):
        return visitor.visitInterface(self)
        
    def loadMethods(self, node):
        for child in node:
            if child.tag == 'method':
                self.methods.append(Function(child, self))
                
class ApiFragment:
    def __init__(self, xmlNode):
        self.name = xmlNode.get('name')
        
        self.types = []
        self.constants = []
        self.globals = []
        self.interfaces = []
        self.structs = []
        self.loadChildren(xmlNode)

    def loadChildren(self, xmlNode):
        for child in xmlNode:
            if child.tag == 'types': self.loadTypes(child)
            elif child.tag == 'constants': self.loadConstants(child)
            elif child.tag == 'structs' : self.loadStructs(child)
            elif child.tag == 'globals': self.loadGlobals(child)
            elif child.tag == 'interfaces': self.loadInterfaces(child)
            
    def loadTypes(self, node):
        for child in node:
            loadedNode = None
            if child.tag == 'typedef': loadedNode = Typedef(child)
            
            if loadedNode is not None:
                self.types.append(loadedNode)

    def loadConstants(self, node):
        for child in node:
            loadedNode = None
            if child.tag == 'enum': loadedNode = Enum(child)
            elif child.tag == 'constant' : loadedNode = Constant(child)

            if loadedNode is not None:
                self.constants.append(loadedNode)

    def loadStructs(self, node):
        for child in node:
            loadedNode = None
            if child.tag == 'struct': loadedNode = Struct(child)

            if loadedNode is not None:
                self.structs.append(loadedNode)

    def loadGlobals(self, node):
        for child in node:
            loadedNode = None
            if child.tag == 'function': loadedNode = Function(child)

            if loadedNode is not None:
                self.globals.append(loadedNode)

    def loadInterfaces(self, node):
        for child in node:
            loadedNode = None
            if child.tag == 'interface': loadedNode = Interface(child)

            if loadedNode is not None:
                self.interfaces.append(loadedNode)
                
class ApiVersion(ApiFragment):
    def __init__(self, xmlNode):
        assert xmlNode.tag == 'version'
        ApiFragment.__init__(self, xmlNode)
    
class ApiExtension:
    def __init__(self, xmlNode):
        assert xmlNode.tag == 'extension'
        ApiFragment.__init__(self, xmlNode)
        
class ApiDefinition:
    def __init__(self, xmlNode):
        assert xmlNode.tag == 'api'
        self.headerFileName = xmlNode.get('headerFile')
        self.typePrefix = xmlNode.get('typePrefix')
        self.constantPrefix = xmlNode.get('constantPrefix')
        self.functionPrefix = xmlNode.get('functionPrefix')

        self.versions = {}
        self.extensions = {}
        self.loadFragments(xmlNode)
        
    def accept(self, visitor):
        return visitor.visitApiDefinition(self)
        
    @staticmethod
    def loadFromFileNamed(filename):
        tree = etree.parse(filename);
        return ApiDefinition(tree.getroot())

    def loadFragments(self, node):
        for c in node:
            if c.tag == 'version':
                version = ApiVersion(c)
                self.versions[version.name] = version
            if c.tag == 'extensions':
                extension = ApiVersion(c)
                self.extensions[extension.name] = extension
                
