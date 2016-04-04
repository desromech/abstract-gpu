# Leng cmake build script.
cmake_minimum_required(VERSION 2.8)
project(AGPU)

option(AGPU_BUILD_SAMPLES "Build AGPU Samples" OFF)

# Check the build type
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Output paths.
set(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

# Turn warnings
if (MSVC)
	# using Visual Studio C++
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fvisibility=hidden")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fvisibility=hidden")
endif()

# Find OpenGL
find_package(OpenGL)

# Find Vulkan
find_path(VULKAN_INCLUDE_DIRS
  NAMES vulkan/vulkan.h
)

# Find Metal
if(APPLE)
	find_library(APPKIT_LIBRARY AppKit)

	find_library(METAL_LIBRARY Metal)
	if(METAL_LIBRARY AND APPKIT_LIBRARY)
		set(METAL_FOUND TRUE)
	endif()
endif()

find_library(VULKAN_LIBRARY NAMES vulkan-1)
if(VULKAN_INCLUDE_DIRS AND VULKAN_LIBRARY)
	set(VULKAN_FOUND TRUE)
	include_directories(${VULKAN_INCLUDE_DIRS})
endif()

if(D3D12_INCLUDE_DIRS AND D3D12_LIBRARY)
	set(D3D12_FOUND TRUE)
endif()

# Add some include directories
include_directories(. include thirdparty/glm thirdparty/opengl/include)

# Set output dir.
set(EXECUTABLE_OUTPUT_PATH "${AGPU_BINARY_DIR}/dist")
set(LIBRARY_OUTPUT_PATH "${AGPU_BINARY_DIR}/dist")

# Try to use pkg-config.
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)

    # Find SDL2
    pkg_check_modules(SDL2 sdl2)

    # Find unittest++
    pkg_check_modules(UNITTESTMM unittest++)
    if(UNITTESTMM_FOUND)
        include_directories(${UNITTESTMM_INCLUDE_DIRS})
    endif()

else()
    # Find Direct3D 12
    find_path(D3D12_INCLUDE_DIRS
      NAMES d3d12.h
    )

    find_library(D3D12_LIBRARY NAMES d3d12)

	if(D3D12_INCLUDE_DIRS AND D3D12_LIBRARY)
		set(D3D12_FOUND TRUE)
	endif()

    # For VisualStudio in Window
    find_path(SDL2_INCLUDE_DIRS
      NAMES SDL.h
    )

    find_path(SDL2_LIBRARY_PATH
      NAMES SDL2.lib SDL2.a
    )

    find_library(SDL2_LIBRARY NAMES SDL2 PATHS ${SDL2_LIBRARY_PATH})
	find_library(SDL2_MAIN_LIBRARY NAMES SDL2main PATHS ${SDL2_LIBRARY_PATH})

	if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARY)
		set(SDL2_FOUND TRUE)
		if(SDL2_MAIN_LIBRARY)
	    	set(SDL2_LIBRARIES ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY})
		else()
			set(SDL2_LIBRARIES ${SDL2_LIBRARY})
		endif()
	endif()
endif()

# Use the SDL2 include path.
include_directories(${SDL2_INCLUDE_DIRS})

# Samples libraries
set(AGPU_MAIN_LIB Agpu)
set(AGPU_SAMPLES_LIBS ${AGPU_SAMPLES_LIBS} ${SDL2_LIBRARIES} ${AGPU_MAIN_LIB})

# Build the implementations
add_subdirectory(implementations)

# Build the samples
if(SDL2_FOUND AND AGPU_BUILD_SAMPLES)
	add_subdirectory(samples)
endif()

# Build the tests.
if(UNITTESTMM_FOUND)
    add_subdirectory(tests)
endif()
