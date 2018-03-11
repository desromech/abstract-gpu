@echo off

cd /d "%~dp0"

echo Appveyor build invoked
echo Appveyor platform %PLATFORM%
build-variant-%PLATFORM%.bat
