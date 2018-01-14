#!/bin/sh

# Use the vulkan SDK
. ./thirdparty/vulkan-sdk/setup-env.sh

echo "Vulkan deps"
echo "ldd ${VULKAN_SDK}/lib/libvulkan.so"
ldd ${VULKAN_SDK}/lib/libvulkan.so

echo "Generating cmake project"
mkdir build
cd build
cmake ..

echo "Building project"
make
