#!/bin/sh

# Use the vulkan SDK
source ./thirdparty/vulkan-sdk/setup-env.sh

echo "G++ version"
g++ --version

echo "G++ 6 version"
g++-6 --version

echo "Generating cmake project"
mkdir build
cd build
cmake ..

echo "Building project"
make
