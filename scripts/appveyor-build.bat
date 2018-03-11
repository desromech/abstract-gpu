@echo off

echo Appveyor build invoked
echo Appveyor platform %PLATFORM%
build-variant-%PLATFORM%.bat
