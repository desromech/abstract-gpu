# AbstractGPU cmake build script.
cmake_minimum_required(VERSION 3.5)
project(AGPU)

if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

option(AGPU_BUILD_SAMPLES "Build AGPU Samples" OFF)
option(BUILD_VULKAN "Build the vulkan backend" ON)
option(BUILD_OPENGL "Build the opengl backend" OFF)
option(BUILD_D3D12 "Build the d3d12 backend" ON)
option(BUILD_METAL "Build the metal backend" ON)
option(BUILD_WITH_OPENVR "Build with OpenVR support" ON)

option(BUILD_MULTIPLE_BACKENDS "Build multiple backends" ON)
option(BUILD_TOOLS "Build the tools" ON)

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	set(UWP YES)
	set(BUILD_VULKAN OFF)
	set(BUILD_OPENGL OFF)
	set(BUILD_D3D12 YES)
	set(BUILD_METAL OFF)
	set(BUILD_WITH_OPENVR OFF)
	set(BUILD_MULTIPLE_BACKENDS OFF)
	set(BUILD_TOOLS OFF)
endif()

# Check the build type
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Output paths.
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

# Turn warnings
if (MSVC)
	# using Visual Studio C++
	#string(REGEX REPLACE "/W3" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	#string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fvisibility=hidden")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fvisibility=hidden")
endif()

set(VULKAN_FIND_INCLUDE_PATHS)
set(VULKAN_FIND_LIBRARY_PATHS)
if(NOT "$ENV{VULKAN_SDK}" STREQUAL "")
	set(VULKAN_FIND_INCLUDE_PATHS "$ENV{VULKAN_SDK}/include")

	if(MSVC)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(VULKAN_FIND_LIBRARY_PATHS "$ENV{VULKAN_SDK}/Lib")
		else()
			set(VULKAN_FIND_LIBRARY_PATHS "$ENV{VULKAN_SDK}/Lib32")
		endif()
	else()
		set(VULKAN_FIND_LIBRARY_PATHS "$ENV{VULKAN_SDK}/lib")
	endif()
endif()

# Find Vulkan
find_path(VULKAN_INCLUDE_DIRS
  NAMES vulkan/vulkan.h
  HINTS ${VULKAN_FIND_INCLUDE_PATHS}
)

# Find Metal
if(APPLE)
	find_library(APPKIT_LIBRARY AppKit)
	find_library(QUARTZCORE_LIBRARY QuartzCore)
	find_library(METAL_LIBRARY Metal)
	if(METAL_LIBRARY AND QUARTZCORE_LIBRARY AND APPKIT_LIBRARY)
		set(METAL_FOUND TRUE)
	endif()

	## Do not build the unneeded openvr support.
	set(BUILD_WITH_OPENVR OFF)
	set(BUILD_VULKAN OFF)
	set(BUILD_OPENGL OFF)
	set(BUILD_D3D12 OFF)
	set(BUILD_METAL ON)
	set(BUILD_WITH_OPENVR OFF)
	set(BUILD_MULTIPLE_BACKENDS OFF)
	set(BUILD_TOOLS OFF)
# We only support vulkan in linux, so do not build the icd and the other backends.
elseif(UNIX)
	set(BUILD_VULKAN ON)
	set(BUILD_OPENGL OFF)
	set(BUILD_D3D12 OFF)
	set(BUILD_METAL OFF)
	set(BUILD_MULTIPLE_BACKENDS OFF)
endif()

find_path(X11_INCLUDE_DIRS
  NAMES Xlib.h
  PATH_SUFFIXES X11
)

find_library(X11_LIBRARY NAMES X11)
find_library(X11_XCB_LIBRARY NAMES X11-xcb)
set(VULKAN_WSYS_LIBRARIES)
set(LOADER_OS_LIBRARIES)
if(X11_LIBRARY AND X11_INCLUDE_DIRS AND NOT APPLE)
    set(LOADER_OS_LIBRARIES ${X11_LIBRARY})
endif()

if(X11_LIBRARY AND X11_XCB_LIBRARY AND NOT APPLE)
    set(VULKAN_WSYS_LIBRARIES ${X11_LIBRARY} ${X11_XCB_LIBRARY})
endif()

find_library(VULKAN_LIBRARY
	NAMES vulkan vulkan-1
	HINTS ${VULKAN_FIND_LIBRARY_PATHS}
)

## Prefer out version of glslang
include_directories(
thirdparty/glslang
thirdparty/spirv-cross/
)

if(VULKAN_INCLUDE_DIRS AND VULKAN_LIBRARY)
	set(VULKAN_FOUND TRUE)
	include_directories(${VULKAN_INCLUDE_DIRS})
endif()

# Add some include directories
include_directories(
.
include
thirdparty/glm
thirdparty/opengl/include
)

# Set output dir.
set(MainOutputDirectory "${AGPU_BINARY_DIR}/dist")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MainOutputDirectory} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MainOutputDirectory} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MainOutputDirectory} )
foreach(Config ${CMAKE_CONFIGURATION_TYPES} )
	set(OutputDirectory "${AGPU_BINARY_DIR}/dist/${Config}")
    string( TOUPPER ${Config} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OutputDirectory} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OutputDirectory} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OutputDirectory} )
endforeach()

# Find unittest++
#pkg_check_modules(UNITTESTMM unittest++)
#if(UNITTESTMM_FOUND)
#    include_directories(${UNITTESTMM_INCLUDE_DIRS})
#endif()

# Find OpenGL
if(BUILD_OPENGL)
	find_package(OpenGL)
endif()

# Find Direct3D 12
if(UWP OR WIN32)
	## Assume we will always have d3d12, so just set the required variables.
	set(D3D12_INCLUDE_DIR)
	set(DXGI_INCLUDE_DIR)
	set(D3DCOMPILER_INCLUDE_DIR)
	set(D3D12_FOUND YES)
	set(D3D12_LIBRARIES d3d12.lib dxgi.lib d3dcompiler.lib)
endif()

# For VisualStudio in Window
find_path(SDL2_INCLUDE_DIRS
  NAMES SDL.h
  PATH_SUFFIXES SDL2
)

find_path(SDL2_LIBRARY_PATH
  NAMES SDL2.lib SDL2.a SDL2-2.0.lib SDL2-2.0.a libSDL2-2.0.a
)

find_library(SDL2_LIBRARY NAMES SDL2-2.0 SDL2 PATHS ${SDL2_LIBRARY_PATH})
find_library(SDL2_MAIN_LIBRARY NAMES SDL2main PATHS ${SDL2_LIBRARY_PATH})

if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARY AND SDL2_INCLUDE_DIRS)
    # Use the SDL2 include path.
    include_directories(${SDL2_INCLUDE_DIRS})

	set(SDL2_FOUND TRUE)
	if(SDL2_MAIN_LIBRARY)
    	set(SDL2_LIBRARIES ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY})
	else()
		set(SDL2_LIBRARIES ${SDL2_LIBRARY})
	endif()
endif()


# Samples libraries
set(AGPU_MAIN_LIB Agpu)
set(AGPU_SAMPLES_LIBS ${AGPU_SAMPLES_LIBS} ${SDL2_LIBRARIES} ${AGPU_MAIN_LIB})

# Build the thirdparty dependencies
add_subdirectory(thirdparty)

# Build the implementations
add_subdirectory(implementations)

# Build the tools
if(NOT UWP)
	add_subdirectory(tools)
endif()

# Build the samples
if(SDL2_FOUND AND AGPU_BUILD_SAMPLES)
	add_subdirectory(samples)
endif()

# Build the tests.
if(UNITTESTMM_FOUND)
    add_subdirectory(tests)
endif()
