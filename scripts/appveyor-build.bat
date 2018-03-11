@echo off

set VULKAN_SDK=c:\projects\agpu\thirdparty\vulkan-sdk-windows

cd /d "%~dp0"

echo Appveyor build invoked
echo Appveyor platform %PLATFORM%
build-variant-%PLATFORM%.bat
