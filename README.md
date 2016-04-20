# AbstractGPU
The Abstract GPU is a cross platform low-level 3D graphics API.
The objective of this project is to create a new 3D graphics API that replaces OpenGL.

# Roadmap
Because the responsibility of creating drivers belongs to the hardware manufacture, this project is
going to start by creating a wrapper around Direct3D 12, Vulkan and Metal.

# Building instruction for Linux and Max OS/X
For building the abstraction layer for Linux and Max OS/X, CMake is required. In
Linux the Vulkan headers and libraries has to be installed. The following commands
can be used for building:

    mkdir build
    cd build
    cmake ..
    make

The built files will be available at the dist folder. The samples will not be
built by default. For building the samples, SDL2 has to be installed and the
AGPU_BUILD_SAMPLES option has to be set. Or for simplicity, you have to use
the following commands for building:

    mkdir build
    cd build
    cmake -DAGPU_BUILD_SAMPLES=True ..
    make

