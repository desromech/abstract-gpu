set(EXECUTABLE_OUTPUT_PATH "${AGPU_BINARY_DIR}/dist/samples")

set(SampleCommon_SRC
    SampleBase.cpp
)

add_executable(Sample1 Sample1.cpp ${SampleCommon_SRC})
target_link_libraries(Sample1 ${AGPU_SAMPLES_LIBS})

add_executable(Sample2 Sample2.cpp ${SampleCommon_SRC})
target_link_libraries(Sample2 ${AGPU_SAMPLES_LIBS})
