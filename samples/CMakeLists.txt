# Data files to distribute.
file(GLOB DATA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "data/shaders/*.glsl"
    "data/shaders/*.hlsl"
    "data/shaders/*.spv"
    "data/shaders/*.metal"
    "data/textures/*.bmp"
)

set(DATA_OUTPUT_PREFIX "${EXECUTABLE_OUTPUT_PATH}")

foreach(data_file ${DATA_FILES})
    add_custom_command(
        OUTPUT "${EXECUTABLE_OUTPUT_PATH}/${data_file}"
        COMMAND cmake -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
            "${EXECUTABLE_OUTPUT_PATH}/${data_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
    )
    list(APPEND data_file_dest "${DATA_OUTPUT_PREFIX}/${data_file}")
endforeach()

if(NOT APPLE)
    add_custom_target(CopyDataFiles ALL DEPENDS ${data_file_dest})
endif()

set(SampleCommon_SRC
    SampleBase.cpp
    SampleBase.hpp
)

add_executable(Sample1 Sample1.cpp ${SampleCommon_SRC})
target_link_libraries(Sample1 ${AGPU_SAMPLES_LIBS})

add_executable(Sample2 Sample2.cpp ${SampleCommon_SRC})
target_link_libraries(Sample2 ${AGPU_SAMPLES_LIBS})

add_executable(Sample3 Sample3.cpp ${SampleCommon_SRC})
target_link_libraries(Sample3 ${AGPU_SAMPLES_LIBS})
