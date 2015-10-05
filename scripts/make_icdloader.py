#!/usr/bin/python
import re
import sys

from definition import *
from string import Template

# Converts text in 'CamelCase' into 'CAMEL_CASE'
# Snippet taken from: http://stackoverflow.com/questions/1175208/elegant-python-function-to-convert-camelcase-to-camel-case
def convertToUnderscore(s):
    return re.sub('(?!^)([0-9A-Z]+)', r'_\1', s).upper().replace('__', '_')

class MakeIcdLoaderVisitor:
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

    def beginSource(self):
        self.writeLine("// This file was generated automatically. DO NOT MODIFY")
        self.writeLine("#include <AGPU/agpu.h>")
        self.newline()

    def visitApiDefinition(self, api):
        self.setup(api)
        self.beginSource();
        self.emitVersions(api.versions)

    def emitVersions(self, versions):
        for version in versions.values():
            self.emitVersion(version)

    def emitVersion(self, version):
        for interface in version.interfaces:
            for method in interface.methods:
                self.emitMethod(method)

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

    def makeArgumentNamesString(self, arguments):
        # Emit void when no having arguments
        if len(arguments) == 0:
            return ''

        result = ''
        for i in range(len(arguments)):
            arg = arguments[i]
            if i > 0: result += ', '
            result += arg.name
        return result

    def emitMethod(self, method):
        allArguments = method.arguments
        assert (method.clazz is not None)
        selfArgument = SelfArgument(method.clazz)
        allArguments = [selfArgument] + allArguments

        arguments = self.makeArgumentsString(allArguments)
        argumentNames = self.makeArgumentNamesString(allArguments)
        self.printLine('${ApiExportMacro} $TypePrefix$ReturnType $FunctionPrefix$FunctionName ( $Arguments )',
            ReturnType = method.returnType,
            FunctionName = method.cname,
            Arguments = arguments)
        self.printLine('{')

        # Check the self argument.
        self.printLine('\tif ($SelfName == nullptr)', SelfName = selfArgument.name)
        if method.returnType == 'error':
            self.printLine('\t\treturn AGPU_NULL_POINTER;')
        else:
            self.printLine('\t\treturn ($TypePrefix$ReturnType)0;', ReturnType = method.returnType)
        self.printLine('\tagpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> ($SelfName);', SelfName = selfArgument.name)
        self.printLine('\treturn (*dispatchTable)->$FunctionPrefix$FunctionName ( $Arguments );',
            FunctionName = method.cname,
            Arguments = argumentNames)
        self.printLine('}')
        self.newline()

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print "make-icdloader <definitions> <output dir>"
    api = ApiDefinition.loadFromFileNamed(sys.argv[1])
    with open(sys.argv[2] + '/redirection.cpp', 'w') as out:
            visitor = MakeIcdLoaderVisitor(out)
            api.accept(visitor)
