$ErrorActionPreference = "Stop"

$BUILD_MODE = $Env:BUILD_MODE
$VS_PLATFORM = $Env:VS_PLATFORM

if(!$BUILD_MODE) {$BUILD_MODE = "Debug"}
if(!$VS_PLATFORM) {$VS_PLATFORM = "Win32"}

mkdir build
cd build
cmake.exe .. -G "Visual Studio 16 2019" -A $VS_PLATFORM
cmake.exe --build . --config $BUILD_MODE
