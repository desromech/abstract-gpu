@echo off

set ARCH=%1
set VS_CODEGEN=%2
set VS_PLATFORM=%3

REM Set the visual studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%

echo %~dp0
cd /d "%~dp0"

REM Got the source directory.
cd ..

REM Create the build directory
if exist "builds\Windows-%ARCH%" rd /Q /S "builds\Windows-%ARCH%"
if not exist "builds\Windows-%ARCH%" mkdir "builds\Windows-%ARCH%"
cd builds\Windows-%ARCH%

REM generate the visual studio solution
cmake -G %VS_CODEGEN% ..\..

REM Build the different versions
msbuild AGPU.sln /p:Configuration=Debug /p:Platform=%VS_PLATFORM%
msbuild AGPU.sln /p:Configuration=RelWithDebInfo /p:Platform=%VS_PLATFORM%
msbuild AGPU.sln /p:Configuration=Release /p:Platform=%VS_PLATFORM%
