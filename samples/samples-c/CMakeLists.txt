set(SampleCommon_SRC
    SampleBase.cpp
    SampleBase.hpp
)

add_executable(Sample-C-1 Sample1.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-C-1 ${AGPU_SAMPLES_LIBS})

add_executable(Sample-C-2 Sample2.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-C-2 ${AGPU_SAMPLES_LIBS})

add_executable(Sample-C-3 Sample3.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-C-3 ${AGPU_SAMPLES_LIBS})

add_executable(Sample-C-Compute-1 SampleCompute1.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-C-Compute-1 ${AGPU_SAMPLES_LIBS})
