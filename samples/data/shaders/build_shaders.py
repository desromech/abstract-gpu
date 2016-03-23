#!/usr/bin/python/

import os
import os.path

ShaderCompiler = "glslangValidator"
ShaderAssembler = "spirv-as"

def buildShader(shaderFileName, type):
    shaderName, extension = os.path.splitext(shaderFileName)
    outputName = shaderName + ".spv"
    tmpName = shaderFileName + "." + type
    commands = """
    copy "%(input)s" "%(tmpName)s"
    %(Compiler)s -V -o "%(output)s" "%(tmpName)s"
    del "%(tmpName)s"
    """ % {"input" : shaderFileName, "tmpName" : tmpName, "output" : outputName, "Compiler" : ShaderCompiler}
    lines = commands.split('\n')
    for line in lines:
        os.system(line)

def buildAssemblyShader(shaderFileName, type):
    shaderName, extension = os.path.splitext(shaderFileName)
    outputName = shaderName + ".spv"
    commands = """
    %(Assembler)s -o "%(output)s" "%(input)s"
    "
    """ % {"input" : shaderFileName, "output" : outputName, "Assembler" : ShaderAssembler}
    lines = commands.split('\n')
    for line in lines:
        os.system(line)

buildShader("simpleFragment.glsl", "frag")
buildShader("simpleVertex.glsl", "vert")
buildAssemblyShader("texturedFragment.as", "frag")
buildShader("texturedVertex.glsl", "vert")
