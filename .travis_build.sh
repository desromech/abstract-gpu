#!/bin/bash

# Use the vulkan SDK, if it exists.
if test -e ./thirdparty/vulkan-sdk; then
    source ./thirdparty/vulkan-sdk/setup-env.sh
fi

#echo "Vulkan deps"
#echo "ldd ${VULKAN_SDK}/lib/libvulkan.so"
#ldd ${VULKAN_SDK}/lib/libvulkan.so

echo "========================================================================="
echo "Generating cmake project"
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_MODE} || exit 1

echo "========================================================================="
echo "Building project"
make || exit 1
