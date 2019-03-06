# Data files to distribute.
file(GLOB DATA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "data/shaders/*.glsl"
    "data/shaders/*.spv"
    "data/textures/*.bmp"
)

set(DATA_OUTPUT_PREFIX "${MainOutputDirectory}")
if(CMAKE_CONFIGURATION_TYPES)
    set(DATA_OUTPUT_PREFIX "${MainOutputDirectory}/$<CONFIG>")
endif()

add_custom_target(SampleData ALL)
foreach(data_file ${DATA_FILES})
    add_custom_command(
        TARGET SampleData
        POST_BUILD
        COMMAND cmake -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
            "${DATA_OUTPUT_PREFIX}/${data_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
        COMMENT "Copy ${data_file}"
    )
endforeach()

add_subdirectory(samples-c)

add_subdirectory(samples-cpp)
