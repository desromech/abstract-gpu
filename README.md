# AbstractGPU
The Abstract GPU is a cross platform low-level 3D graphics API.
The objective of this project is to create a new 3D graphics API that replaces OpenGL.

# Roadmap
Because the responsibility of creating drivers belongs to the hardware manufacture, this project is
going to start by creating a wrapper around OpenGL and Direct3D 10/11 and a new shading language that
compiles into GLSL and HLSL.

After having a working API, the following objective is to create a Gallium state tracker.
