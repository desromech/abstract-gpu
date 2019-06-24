#!/bin/bash

OutFile="compiled.inc"

echo "// Compiled shader binary data. Do not modify" > $OutFile
for F in *.{frag,vert}; do
    VarName="$(echo $F | sed '')"
    glslc -o "$F.spv" "$F"
    echo "// $F" >> $OutFile
    xxd -i "$F.spv" | sed 's/unsigned /static unsigned /g'>> $OutFile
    echo >> $OutFile
done

rm ./*.spv
