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
    %(Compiler)s -V -S %(Stage)s -o "%(output)s" "%(input)s"
    """ % {"input" : shaderFileName, "Stage" : type, "output" : outputName, "Compiler" : ShaderCompiler}
    lines = commands.split('\n')
    for line in lines:
        os.system(line)

buildShader("simpleFragment.glsl", "frag")
buildShader("simpleVertex.glsl", "vert")
buildShader("texturedFragment.glsl", "frag")
buildShader("texturedVertex.glsl", "vert")
buildShader("computeAdd.glsl", "comp")
