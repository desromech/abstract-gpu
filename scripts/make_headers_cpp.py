#!/usr/bin/python
import re
import sys

from definition import *
from string import Template


HEADER_START = \
"""
#ifndef $HeaderProtectMacro
#define $HeaderProtectMacro

#include <stdexcept>
#include "AGPU/$CHeader"

/**
 * Abstract GPU exception.
 */
class agpu_exception : public std::runtime_error
{
public:
    explicit agpu_exception(agpu_error error)
        : std::runtime_error("AGPU Error"), errorCode(error)
    {
    }
    
    agpu_error getErrorCode() const
    {
        return errorCode;
    }
    
private:
    agpu_error errorCode;
};

/**
 * Abstract GPU reference smart pointer.
 */
template<typename T>
class agpu_ref
{
public:
    agpu_ref()
        : pointer(0)
    {
    }
    
    agpu_ref(const agpu_ref<T*> &other)
    {
        if(other.pointer)
            other.pointer->addReference();
        pointer = other.pointer();
    }
    
    agpu_ref(T* pointer)
        : pointer(pointer)
    {
    }

    agpu_ref<T> &operator=(const agpu_ref<T*> &other)
    {
        if(pointer != other.pointer)
        {
            if(other.pointer)
                other.pointer->addReference();
            if(pointer)
                pointer->release();
            pointer = other.pointer;
        }
        return *this;
    }
    
    operator bool() const
    {
        return pointer;
    }
    
    bool operator!() const
    {
        return !pointer;
    }
    
    T* get() const
    {
        return pointer;
    }
    
    T *operator->() const
    {
        return pointer;
    }
    
private:
    T *pointer;
};

/**
 * Helper function to convert an error code into an exception.
 */
inline void AgpuThrowIfFailed(agpu_error error)
{
    if(error_code < 0)
        throw agpu_exception(error);
}

"""

HEADER_END = \
"""
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
            'HeaderProtectMacro' : (api.headerFileName + 'pp').upper().replace('.', '_') + '_' ,
            'CHeader' : api.headerFileName ,
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
        
    def emitMethodWrapper(self, function):
        allArguments = function.arguments

        arguments = self.makeArgumentsString(allArguments)
        paramNames = self.makeArgumentNamesString(allArguments)
        if function.returnType == 'error':
            self.printLine('\tinline void $FunctionName ( $Arguments )',
                ReturnType = function.returnType,
                FunctionName = function.name,
                Arguments = arguments)
            self.printLine('\t{')
            self.printLine('\t\tAgpuThrowIfFailed($FunctionPrefix$FunctionName( $Arguments ));', FunctionName = function.cname, Arguments = paramNames)
            self.printLine('\t}')
            self.newline()
        else:
            self.printLine('\tinline $TypePrefix$ReturnType $FunctionName ( $Arguments )',
                ReturnType = function.returnType,
                FunctionName = function.name,
                Arguments = arguments)
            self.printLine('\t{')
            self.printLine('\t\treturn $FunctionPrefix$FunctionName( $Arguments );', FunctionName = function.cname, Arguments = paramNames)
            self.printLine('\t}')
            self.newline()
        
    def makeArgumentsString(self, arguments):
        # Emit void when no having arguments
        if len(arguments) == 0:
            return ''

        result = ''
        for i in range(len(arguments)):
            arg = arguments[i]
            if i > 0: result += ', '
            result += self.processText('$TypePrefix$Type $Name', Type = arg.type, Name = arg.name)
        return result
        
    def makeArgumentNamesString(self, arguments):
        result = 'this'
        for i in range(len(arguments)):
            arg = arguments[i]
            result += ', %s' % arg.name
        return result
  
    def emitInterface(self, interface):
        self.printLine('// Interface wrapper for $TypePrefix$Name.', Name = interface.name)
        self.printLine('struct $TypePrefix$Name', Name = interface.name)
        self.printLine('{')
        self.printLine('private:')
        self.printLine('\t$TypePrefix$Name() {}', Name = interface.name)
        self.newline()
        self.printLine('public:')
        for method in interface.methods:
            self.emitMethodWrapper(method)
        self.printLine('};')
        
        self.newline()

    def emitFragment(self, fragment):
        # Emit the interface methods
        for interface in fragment.interfaces:
            self.emitInterface(interface)

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
    with open(sys.argv[2] + '/' + api.headerFileName + 'pp', 'w') as out:
        visitor = MakeHeaderVisitor(out)
        api.accept(visitor)