if(BUILD_MULTIPLE_BACKENDS)
	add_subdirectory(Loader)

	set(MainOutputDirectory "${AGPU_BINARY_DIR}/dist/AgpuIcd")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MainOutputDirectory} )
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MainOutputDirectory} )
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MainOutputDirectory} )
	foreach(Config ${CMAKE_CONFIGURATION_TYPES} )
		set(OutputDirectory "${AGPU_BINARY_DIR}/dist/${Config}/AgpuIcd")
	    string( TOUPPER ${Config} OUTPUTCONFIG )
	    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OutputDirectory} )
	    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OutputDirectory} )
	    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OutputDirectory} )
	endforeach()
endif()

add_subdirectory(Common)

if(D3D12_FOUND AND BUILD_D3D12)
    add_subdirectory(Direct3D12)
endif()

if(VULKAN_FOUND AND BUILD_VULKAN)
  add_subdirectory(Vulkan)
endif()

if(OPENGL_FOUND AND NOT APPLE AND BUILD_OPENGL)
    add_subdirectory(OpenGL)
endif()

if(METAL_FOUND AND BUILD_METAL)
  add_subdirectory(Metal)
endif()
