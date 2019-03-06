set(SampleCommon_SRC
    SampleBase.cpp
    SampleBase.hpp
)

add_executable(Sample-Cpp-1 Sample1.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-Cpp-1 ${AGPU_SAMPLES_LIBS})

add_executable(Sample-Cpp-2 Sample2.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-Cpp-2 ${AGPU_SAMPLES_LIBS})

add_executable(Sample-Cpp-3 Sample3.cpp ${SampleCommon_SRC})
target_link_libraries(Sample-Cpp-3 ${AGPU_SAMPLES_LIBS})
