import re

from definition import *
from string import Template

HEADER_START = \
"""
#ifndef $HeaderProtectMacro
#define $HeaderProtectMacro

#include <stdlib.h>

#ifdef __cplusplus
#define extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
#   ifdef ${ConstantPrefix}BUILD
#       define ${ApiExportMacro} __declspec(dllexport)
#   else
#       define ${ApiExportMacro} __declspec(dllimport)
#   endif
#else
#   define ${ApiExportMacro}
#endif

"""

HEADER_END = \
"""
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* $HeaderProtectMacro */
"""

# Converts text in 'CamelCase' into 'CAMEL_CASE'
# Snippet taken from: http://stackoverflow.com/questions/1175208/elegant-python-function-to-convert-camelcase-to-camel-case
def convertToUnderscore(s):
    return re.sub('(?!^)([0-9A-Z]+)', r'_\1', s).upper()
    
class MakeHeaderVisitor:
    def __init__(self, out):
        self.out = out
        self.variables = {}

    def processText(self, text, **extraVariables):
        t = Template(text)
        return t.substitute(**dict(self.variables.items() + extraVariables.items()))
        
    def write(self, text):
        self.out.write(text)
        
    def writeLine(self, line):
        self.write(line)
        self.newline()
        
    def newline(self):
        self.write('\n')
        
    def printString(self, text, **extraVariables):
        self.write(self.processText(text, **extraVariables))
        
    def printLine(self, text, **extraVariables):
        self.write(self.processText(text, **extraVariables))
        self.newline()
        
    def setup(self, api):
        self.api = api
        self.variables ={
            'HeaderProtectMacro' : '_' + api.headerFileName.upper().replace('.', '_') + '_' ,
            'ApiExportMacro': api.constantPrefix + 'EXPORT', 
            'ConstantPrefix' : api.constantPrefix,
            'FunctionPrefix' : api.functionPrefix,
            'TypePrefix' : api.typePrefix,
        }
        
    def beginHeader(self):
        self.printString(HEADER_START)
        
    def endHeader(self):
        self.printString(HEADER_END)
        
    def visitApiDefinition(self, api):
        self.setup(api)
        self.beginHeader();
        self.emitVersions(api.versions)
        self.emitExtensions(api.extensions)
        self.endHeader();
        
    def visitTypedef(self, typedef):
        self.printLine('typedef $CType $TypePrefix$Name;', CType=typedef.ctype, Name=typedef.name)
        
    def visitConstant(self, constant):
        self.printLine('#define $ConstantPrefix$ConstantName (($CType)$ConstantValue)',
            ConstantName = convertToUnderscore(constant.name),
            CType = constant.ctype, ConstantValue = str(constant.value))
        
    def visitEnum(self, enum):
        self.printLine('typedef enum {')
        for constant in enum.constants:
            self.printLine("\t$ConstantPrefix$ConstantName = $ConstantValue,", ConstantName=convertToUnderscore(constant.name), ConstantValue= str(constant.value))
        self.printLine('} $TypePrefix$EnumName;', EnumName=enum.name)
        self.newline()

    def emitFunctionPointerType(self, function):
        allArguments = function.arguments
        if function.clazz is not None:
            allArguments = [SelfArgument(function.clazz)] + allArguments
            
        arguments = self.makeArgumentsString(allArguments)
        self.printLine('typedef $TypePrefix$ReturnType (*$FunctionPrefix${FunctionName}_FUN) ( $Arguments );',
            ReturnType = function.returnType,
            FunctionName = function.cname,
            Arguments = arguments)
        
    def emitFunction(self, function):
        allArguments = function.arguments
        if function.clazz is not None:
            allArguments = [SelfArgument(function.clazz)] + allArguments
            
        arguments = self.makeArgumentsString(allArguments)
        self.printLine('${ApiExportMacro} $TypePrefix$ReturnType $FunctionPrefix$FunctionName ( $Arguments );',
            ReturnType = function.returnType,
            FunctionName = function.cname,
            Arguments = arguments)
        
    def makeArgumentsString(self, arguments):
        # Emit void when no having arguments
        if len(arguments) == 0:
            return 'void'

        result = ''
        for i in range(len(arguments)):
            arg = arguments[i]
            if i > 0: result += ', '
            result += self.processText('$TypePrefix$Type $Name', Type = arg.type, Name = arg.name)
        return result
        
    def emitGlobals(self, functions):
        self.writeLine('/* Global functions. */')
        for function in functions:
            self.emitFunctionPointerType(function)
        self.newline()
        
        for function in functions:
            self.emitFunction(function)
        self.newline()
    
    def emitInterface(self, interface):
        self.printLine('/* Methods for interface $TypePrefix$Name. */', Name = interface.name)
        for method in interface.methods:
            self.emitFunctionPointerType(method)
        self.newline()
        
        for method in interface.methods:
            self.emitFunction(method)
        self.newline()
        
    def emitFragment(self, fragment):
        # Emit the types
        for typ in fragment.types:
            typ.accept(self)
        self.newline()
            
        # Emit the constants.
        for constant in fragment.constants:
            constant.accept(self)
        self.newline()
        
        # Emit the global functions
        self.emitGlobals(fragment.globals)
        
        # Emit the interface methods
        for interface in fragment.interfaces:
            self.emitInterface(interface)
        self.newline()
        
    def emitVersion(self, version):
        self.emitFragment(version)
        
    def emitVersions(self, versions):
        for version in versions.values():
            self.emitVersion(version)

    def emitExtension(self, version):
        self.emitFragment(version)
        
    def emitExtensions(self, extensions):
        for extension in extensions.values():
            self.emitExtension(extension)

if __name__ == '__main__':
    api = ApiDefinition.loadFromFileNamed('../definitions/api.xml')
    with open(api.headerFileName, 'w') as out:
        visitor = MakeHeaderVisitor(out)
        api.accept(visitor)
