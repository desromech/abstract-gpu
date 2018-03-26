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

add_executable(SampleCompute1 SampleCompute1.cpp ${SampleCommon_SRC})
target_link_libraries(SampleCompute1 ${AGPU_SAMPLES_LIBS})

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

foreach(data_file ${DATA_FILES})
    add_custom_command(
        TARGET Sample1
        POST_BUILD
        COMMAND cmake -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
            "${DATA_OUTPUT_PREFIX}/${data_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
        COMMENT "Copy ${data_file}"
    )
endforeach()
