#!/usr/bin/python
import re
import sys

from definition import *
from string import Template

HEADER_START = \
"""
#ifndef $HeaderProtectMacro
#define $HeaderProtectMacro

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
#   ifdef ${ConstantPrefix}BUILD
#       define ${ApiExportMacro} __declspec(dllexport)
#   else
#       define ${ApiExportMacro} __declspec(dllimport)
#   endif
#else
#   if __GNUC__ >= 4
#       define ${ApiExportMacro} __attribute__ ((visibility ("default")))
#   endif
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

    def emitInterfaceType(self, interface):
        self.printLine('typedef struct _$TypePrefix$Name $TypePrefix$Name;', Name = interface.name)
    
    def emitInterface(self, interface):
        self.printLine('/* Methods for interface $TypePrefix$Name. */', Name = interface.name)
        for method in interface.methods:
            self.emitFunctionPointerType(method)
        self.newline()
        
        for method in interface.methods:
            self.emitFunction(method)
        self.newline()

    def emitField(self, field):
        self.printLine('\t$TypePrefix$Type $Name;', Type = field.type, Name = field.name)
        
    def emitStruct(self, struct):
        self.printLine('/* Structure $TypePrefix$Name. */', Name = struct.name)
        self.printLine('typedef struct $TypePrefix$Name {', Name = struct.name)
        for field in struct.fields:
            self.emitField(field)
        self.printLine('} $TypePrefix$Name;', Name = struct.name)
        self.newline()

    def emitIcdInterface(self, fragment):
        self.printLine('/* Installable client driver interface. */')
        self.printLine('typedef struct _agpu_icd_dispatch {')
        self.printLine('\tint icd_interface_version;')
        for function in fragment.globals:
            self.printLine('\t$FunctionPrefix${FunctionName}_FUN $FunctionPrefix${FunctionName};', FunctionName = function.cname)

        for interface in fragment.interfaces:
            for method in interface.methods:
                self.printLine('\t$FunctionPrefix${FunctionName}_FUN $FunctionPrefix${FunctionName};', FunctionName = method.cname)

        self.printLine('} agpu_icd_dispatch;')

    def emitFragment(self, fragment):
        # Emit the types
        for typ in fragment.types:
            typ.accept(self)
        self.newline()

        # Emit the interface types
        for interface in fragment.interfaces:
            self.emitInterfaceType(interface)
        self.newline()

        # Emit the constants.
        for constant in fragment.constants:
            constant.accept(self)
        self.newline()

        # Emit the structures.
        for struct in fragment.structs:
            self.emitStruct(struct)
        
        # Emit the global functions
        self.emitGlobals(fragment.globals)
        
        # Emit the interface methods
        for interface in fragment.interfaces:
            self.emitInterface(interface)

        # Emit the icd interface
        self.emitIcdInterface(fragment)
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
    if len(sys.argv) < 3:
        print "make-headers <definitions> <output dir>"
    api = ApiDefinition.loadFromFileNamed(sys.argv[1])
    with open(sys.argv[2] + '/' + api.headerFileName, 'w') as out:
        visitor = MakeHeaderVisitor(out)
        api.accept(visitor)
