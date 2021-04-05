$ErrorActionPreference = "Stop"

$BUILD_MODE = $Env:BUILD_MODE
$VS_PLATFORM = $Env:VS_PLATFORM

if(!$BUILD_MODE) {$BUILD_MODE = "Debug"}
if(!$VS_PLATFORM) {$VS_PLATFORM = "Win32"}

# Use the bundled vulkan SDK.
$Env:VULKAN_SDK = "$(pwd)\thirdparty\vulkan-sdk-windows"

mkdir build
cd build
echo "cmake.exe .. -G ""Visual Studio 16 2019"" -A $VS_PLATFORM"
cmake.exe .. -G "Visual Studio 16 2019" -A $VS_PLATFORM
echo "cmake.exe --build . --config $BUILD_MODE"
cmake.exe --build . --config $BUILD_MODE
