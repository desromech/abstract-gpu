#!/usr/bin/python/

import os
import os.path

ShaderCompiler = "glslangValidator -V"

def buildShader(shaderFileName, type):
    shaderName, extension = os.path.splitext(shaderFileName)
    outputName = shaderName + ".spv"
    tmpName = shaderFileName + "." + type
    commands = """
    copy "%(input)s" "%(tmpName)s"
    %(Compiler)s -o "%(output)s" "%(tmpName)s"
    del "%(tmpName)s"
    """ % {"input" : shaderFileName, "tmpName" : tmpName, "output" : outputName, "Compiler" : ShaderCompiler}
    lines = commands.split('\n')
    for line in lines:
        os.system(line)

buildShader("simpleFragment.glsl", "frag")
buildShader("simpleVertex.glsl", "vert")
buildShader("texturedFragment.glsl", "frag")
buildShader("texturedVertex.glsl", "vert")
